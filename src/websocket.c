#include "websocket.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <nettle/base64.h>
#include <nettle/sha.h>


ssize_t websocket_send_callback(wslay_event_context_ptr ctx, const uint8_t *data, size_t len, int flags, void *user_data) {
    int* fd = (int*)user_data;
    ssize_t r;
    int sflags = 0;
#ifdef MSG_MORE
    if(flags & WSLAY_MSG_MORE) {
        sflags |= MSG_MORE;
    }
#endif // MSG_MORE
    while((r = send(*fd, data, len, sflags)) == -1 && errno == EINTR);
    if(r == -1) {
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            wslay_event_set_error(ctx, WSLAY_ERR_WOULDBLOCK);
        } else {
            wslay_event_set_error(ctx, WSLAY_ERR_CALLBACK_FAILURE);
        }
    }
    return r;
}


int websocket_send(socket_t sock, unsigned char *buff, int n, wslay_event_context_ptr ctx) {
#if 1 // binary
    struct wslay_event_msg msg = { WSLAY_BINARY_FRAME, buff, n};
#else // text
    struct wslay_event_msg msg = { WSLAY_TEXT_FRAME, buf, strlen(buf)};
#endif
    int res = wslay_event_queue_msg(ctx, &msg);
    if(res < 0) {
        fprintf(stderr, "Failed to queue\n");
        return -1;
    }

    struct pollfd event;
    memset(&event, 0, sizeof(struct pollfd));
    event.fd = sock;
    event.events = POLLOUT;
    while(wslay_event_want_write(ctx)) {
        int r;
        while((r = poll(&event, 1, 0)) == -1 && errno == EINTR);
        if(r == -1) {
            perror("poll");
            return -1;
        }
        if(((event.revents & POLLOUT) && wslay_event_send(ctx) != 0) ||
            (event.revents & (POLLERR | POLLHUP | POLLNVAL))) {
            return -1;
        }
    }

    return n;
}


/*
 * Calculates SHA-1 hash of *src*. The size of *src* is *src_length* bytes.
 * *dst* must be at least SHA1_DIGEST_SIZE.
 */
static void sha1(uint8_t *dst, const uint8_t *src, size_t src_length) {
    struct sha1_ctx ctx;
    sha1_init(&ctx);
    sha1_update(&ctx, src_length, src);
    sha1_digest(&ctx, SHA1_DIGEST_SIZE, dst);
}


/*
 * Base64-encode *src* and stores it in *dst*.
 * The size of *src* is *src_length*.
 * *dst* must be at least BASE64_ENCODE_RAW_LENGTH(src_length).
 */
static void base64(uint8_t *dst, const uint8_t *src, size_t src_length) {
    struct base64_encode_ctx ctx;
    base64_encode_init(&ctx);
    base64_encode_raw((char *)dst, src_length, src);
}


#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"


/*
 * Create Server's accept key in *dst*.
 * *client_key* is the value of |Sec-WebSocket-Key| header field in
 * client's handshake and it must be length of 24.
 * *dst* must be at least BASE64_ENCODE_RAW_LENGTH(20)+1.
 */
static void create_accept_key(char *dst, const char *client_key) {
    uint8_t sha1buf[20], key_src[60];
    memcpy(key_src, client_key, 24);
    memcpy(key_src+24, WS_GUID, 36);
    sha1(sha1buf, key_src, sizeof(key_src));
    base64((uint8_t*)dst, sha1buf, 20);
    dst[BASE64_ENCODE_RAW_LENGTH(20)] = '\0';
}


/* We parse HTTP header lines of the format
 *   \r\nfield_name: value1, value2, ... \r\n
 *
 * If the caller is looking for a specific value, we return a pointer to the
 * start of that value, else we simply return the start of values list.
 */
static const char* http_header_find_field_value(const char *header, char *field_name, char *value) {
    const char* field_end;
    char *next_crlf, *value_start;
    int field_name_len;

    /* Pointer to the last character in the header */
    const char* header_end = header + strlen(header) - 1;

    field_name_len = strlen(field_name);

    const char* field_start = header;

    do {
        field_start = strstr(field_start+1, field_name);

        field_end = field_start + field_name_len - 1;

        if(field_start != NULL
            && field_start - header >= 2
            && field_start[-2] == '\r'
            && field_start[-1] == '\n'
            && header_end - field_end >= 1
            && field_end[1] == ':') {
            break; /* Found the field */
        } else {
            continue; /* This is not the one; keep looking. */
        }
    } while(field_start != NULL);

    if(field_start == NULL) return NULL;

    /* Find the field terminator */
    next_crlf = strstr(field_start, "\r\n");

    /* A field is expected to end with \r\n */
    if(next_crlf == NULL) return NULL; /* Malformed HTTP header! */

    /* If not looking for a value, then return a pointer to the start of values string */
    if(value == NULL) return field_end+2;

    value_start = strstr(field_start, value);

    /* Value not found */
    if(value_start == NULL) return NULL;

    /* Found the value we're looking for */
    if(value_start > next_crlf) return NULL; /* ... but after the CRLF terminator of the field. */

    /* The value we found should be properly delineated from the other tokens */
    if(isalnum(value_start[-1]) || isalnum(value_start[strlen(value)])) return NULL;

    return value_start;
}


int websocket_check_http_header(const char* header, char* response, int max_response_len) {
    char accept_key[29];
    const char *keyhdstart, *keyhdend;

    if(http_header_find_field_value(header, "Upgrade", "websocket") == NULL ||
       http_header_find_field_value(header, "Connection", "Upgrade") == NULL ||
       (keyhdstart = http_header_find_field_value(header, "Sec-WebSocket-Key", NULL)) == NULL) {
        fprintf(stderr, "HTTP Handshake: Missing required header fields");
        return -1;
    }
    for(; *keyhdstart == ' '; ++keyhdstart);
    keyhdend = keyhdstart;
    for(; *keyhdend != '\r' && *keyhdend != ' '; ++keyhdend);
    if(keyhdend-keyhdstart != 24) {
        printf("%s\n", keyhdstart);
        fprintf(stderr, "HTTP Handshake: Invalid value in Sec-WebSocket-Key");
        return -1;
    }
    create_accept_key(accept_key, keyhdstart);
    snprintf(response, max_response_len,
           "HTTP/1.1 101 Switching Protocols\r\n"
           "Upgrade: websocket\r\n"
           "Connection: Upgrade\r\n"
           "Sec-WebSocket-Accept: %s\r\n"
           "\r\n", accept_key);
    return strlen(response);
}
