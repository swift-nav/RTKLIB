#ifndef _WEBSOCKET_H_
#define _WEBSOCKET_H_

#include <wslay/wslay.h>

#ifdef WIN32
#define socket_t            SOCKET
#else
#define socket_t            int
#endif

ssize_t websocket_send_callback(wslay_event_context_ptr ctx, const uint8_t *data, size_t len, int flags, void *user_data);
int websocket_send(socket_t sock, unsigned char *buff, int n, wslay_event_context_ptr ctx);
int websocket_check_http_header(const char* header, char* response, int max_response_len);

#endif /* _WEBSOCKET_H_ */
