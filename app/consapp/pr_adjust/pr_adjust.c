/*------------------------------------------------------------------------------
* pr_adjust.c : apply DGNSS corrections to a stream of rover observations
*
*          Copyright (C) 2023 Swift Navigation
*
* Inputs:
* 1. A file containing rover observations in RTCM3 format
* 2. A file containing DGNSS corrections in RTCM3 format
*
* Outputs:
* 1. A file containing adjusted pseudoranges in RTCM3 format
*
* See the document "RTCM Usage Guidelines - MSM1" for further information.
*
* Loosely based on rnx2rtkp and convbin.
*-----------------------------------------------------------------------------*/
#include <stdarg.h>
#include "rtklib.h"

#define PROGNAME    "pr_adjust"         /* program name */

/*
 * This program depends upon two functions which are not exposed via the
 * official RTKLIB API.  In order to be able to successfully link,
 * it is necessary to change these two prototypes from 'static' to 'extern'
 * in their respective source files.
 */

// from pntpos.c
extern double gettgd(int sat, const nav_t* nav, int type);

// from rtkpos.c
extern int selsat(const obsd_t* obs, double* azel, int nu, int nr, const prcopt_t* opt, int* sat, int* iu, int* ir);


/* help text -----------------------------------------------------------------*/
static const char* help[]={
"",
" Apply DGNSS corrections to a stream of rover observations.",
"",
" Usage:",
"",
"     pr_adjust [option ...] <rover stream> <correction stream>",
"",
" Input and output files should contain data in RTCM3 format.",
"",
" Options [default]:",
"",
" -tr y/m/d h:m:s  approximated time for RTCM/CMR/CMR+ messages",
" -o file   set output file",
" -sys s[,s...] nav system(s) (s=G:GPS,R:GLO,E:GAL,J:QZS,C:BDS,I:IRN) [G|E|C]",
" -f freq   number of frequencies for relative mode (1:L1,2:L1+L2,3:L1+L2+L5) [3]",
" -x level  debug trace level (0:off) [0]"
};

extern int showmsg(const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);
    fprintf(stderr, "\r");
    return 0;
}

extern void settspan(gtime_t ts, gtime_t te) {}
extern void settime(gtime_t time) {}

static void printhelp() {
    int i;
    for(i = 0; i< (int)(sizeof(help)/sizeof(*help)); i++) {
        fprintf(stderr, "%s\n", help[i]);
    }
    exit(0);
}


static double get_time_group_delay(const obsd_t* obs, int freq, const nav_t* nav) {
    double gamma, tgd;
    int sat = obs->sat;
    int code = obs->code[freq];

    switch(satsys(sat, NULL)) {
        case SYS_GPS:
            switch(code) {
                case CODE_L5I:
                case CODE_L5Q:
                case CODE_L5X: // L5
                    gamma = (FREQ1/FREQ5) * (FREQ1/FREQ5);
                    tgd = gamma * gettgd(sat, nav, 1); /* TGD (m) */
                    break;
                case CODE_L2C:
                case CODE_L2D:
                case CODE_L2S:
                case CODE_L2L:
                case CODE_L2X:
                case CODE_L2P:
                case CODE_L2W:
                case CODE_L2Y:
                case CODE_L2M:
                case CODE_L2N: // L2
                    gamma = (FREQ1/FREQ2) * (FREQ1/FREQ2);
                    tgd = gamma * gettgd(sat, nav, 0); /* TGD (m) */
                    break;
                default: // L1
                    tgd = gettgd(sat, nav, 0); /* TGD (m) */
                    break;
            }
            return tgd;
        case SYS_QZS:
            switch(code) {
                case CODE_L5D:
                case CODE_L5P:
                case CODE_L5Z:
                case CODE_L5I:
                case CODE_L5Q:
                case CODE_L5X: // L5
                    gamma = (FREQ1/FREQ5) * (FREQ1/FREQ5);
                    tgd = gamma * gettgd(sat, nav, 1); /* TGD (m) */
                    break;
                case CODE_L2S:
                case CODE_L2L:
                case CODE_L2X: // L2
                    gamma = (FREQ1/FREQ2) * (FREQ1/FREQ2);
                    tgd = gamma * gettgd(sat, nav, 0); /* TGD (m) */
                    break;
                default: // L1
                    tgd = gettgd(sat, nav, 0); /* TGD (m) */
                    break;
            }
            return tgd;
        case SYS_GLO:
            gamma = (FREQ1_GLO/FREQ2_GLO) * (FREQ1_GLO/FREQ2_GLO);
            tgd = gettgd(sat, nav, 0); /* -dtaun (m) */
            return tgd/(gamma-1.0);
        case SYS_GAL:
            switch(code) {
                case CODE_L5I:
                case CODE_L5Q:
                case CODE_L5X: // E5a
                    gamma = (FREQ1/FREQ5) * (FREQ1/FREQ5);
                    tgd = gamma * gettgd(sat, nav, 0); // BGD_E1E5a
                    break;
                case CODE_L7I:
                case CODE_L7Q:
                case CODE_L7X: // E5b
                    gamma = (FREQ1/FREQ7) * (FREQ1/FREQ7);
                    tgd = gamma * gettgd(sat, nav, 1); // BGD_E1E5b
                    break;
                default: // E1
                    tgd = gettgd(sat, nav, 1); // BGD_E1E5b
                    break;
            }
            return tgd;
        case SYS_CMP:
            switch(code) {
                case CODE_L2I: // TGD_B1I
                    tgd = gettgd(sat, nav, 0);
                    break;
                case CODE_L7I:
                case CODE_L7Q:
                case CODE_L7X:
                case CODE_L7D:
                case CODE_L7P:
                case CODE_L7Z: // TGD_B2I/B2b
                    tgd = gettgd(sat, nav, 1);
                    break;
                case CODE_L1P: // TGD_B1Cp
                    tgd = gettgd(sat, nav, 2);
                    break;
                case CODE_L5P: // TGD_B2ap
                    tgd = gettgd(sat, nav, 3);
                    break;
                case CODE_L5X:
                case CODE_L8X:
                case CODE_L5D: // ISC_B2ad
                    tgd = gettgd(sat, nav, 5); // ISC_B2ad
                    break;
                default: // TGD_B1Cp+ISC_B1Cd
                    tgd = gettgd(sat, nav, 2) + gettgd(sat, nav, 4);
                    break;
            }
            return tgd;
        case SYS_IRN: // L5
            gamma = (FREQ9/FREQ5) * (FREQ9/FREQ5);
            tgd = gettgd(sat,nav,0); /* TGD (m) */
            return gamma * tgd;
    }

    return 0.;
}


/*
 * adjust_pseudoranges
 *
 * Given a set of rover observations and a set of base observations, compute
 * the PRC (Pseudo Range Correction) value for each satellite and adjust each
 * observation accordingly
 */
static void adjust_pseudoranges(const prcopt_t* opt, obsd_t* obs, int num_obs_rover, int num_obs_base, const nav_t* nav) {
    int i;
    double *rs,*dts,*var,*azel;
    double basepos_llh[3]; // base station position as lat/lon/alt
    int num_obs_matching;
    int matching_sats[MAXSAT]; // matching satellites between base and rover
    int iu[MAXSAT]; // indices of matching rover obs
    int ir[MAXSAT]; // indices of matching base obs
    int svh[MAXOBS*2]; // satellite health info
    int num_obs_total = num_obs_rover + num_obs_base;
    double e[3]; // LOS unit vector
    int idx_rover, idx_base, idx_matching;
    double geometric_range, pseudorange;

    trace(3, "adjust_pseudorange: time=%s n=%d\n", time_str(obs[0].time, 3), num_obs_total);
    trace(4, "obs in=\n"); traceobs(4, obs, num_obs_total);

    rs = mat(6, num_obs_total); dts = mat(2, num_obs_total); var = mat(1, num_obs_total); azel = zeros(2, num_obs_total);

    // compute satellite positions/clocks at time of transmission
    satposs(obs[0].time, obs, num_obs_total, nav, EPHOPT_BRDC, rs, dts, var, svh);

    // compute geometry values for all satellites visible from base station
    ecef2pos(opt->rb, basepos_llh);
    for(idx_base = 0; idx_base < num_obs_base; idx_base++) {
        if(geodist(rs + num_obs_rover*6 + idx_base*6, opt->rb, e) <= 0.) {
            continue;
        }
        satazel(basepos_llh, e, azel + num_obs_rover*2 + idx_base*2);
    }

    // select common satellites between rover and base station
    if((num_obs_matching = selsat(obs, azel, num_obs_rover, num_obs_base, opt, matching_sats, iu, ir)) <= 0) {
        trace(1, "no common satellites\n");

        free(rs); free(dts); free(var); free(azel);
        return;
    }

    // iterate over all rover observations
    idx_matching = 0;
    for(idx_rover = 0; idx_rover < num_obs_rover; idx_rover++) {
        if(idx_matching < num_obs_matching && iu[idx_matching] == idx_rover) { // have matching base observations
            idx_base = ir[idx_matching++];

            // exclude satellite due to bad health?
            if(satexclude(obs[idx_rover].sat, var[idx_rover], svh[idx_rover], opt) ||
               satexclude(obs[idx_base].sat, var[idx_base], svh[idx_base], opt)) {
                trace(3, "unhealthy sat %i\n", obs[idx_rover].sat);
                for(i = 0; i < opt->nf; i++) { // invalidate
                    obs[idx_rover].P[i] = 0.;
                }
                continue;
            }

            // geometric range from base station to satellite (corrected for Sagnac effect)
            geometric_range = geodist(rs + idx_base*6, opt->rb, e);

            if(geometric_range <= 0.) {
                trace(3, "invalid geometric range for base sat %i\n", obs[idx_base].sat);
                for(i = 0; i < opt->nf; i++) { // invalidate
                    obs[idx_rover].P[i] = 0.;
                }
                continue;
            }

            // satellite clock bias (corrected for relativistic effect)
            geometric_range -= CLIGHT * dts[idx_base*2];

            for(i = 0; i < opt->nf; i++) {
                if(obs[idx_rover].P[i] == 0. || obs[idx_base].P[i] == 0.) {
                    obs[idx_rover].P[i] = 0.;
                    continue;
                }

                pseudorange = obs[idx_base].P[i] - get_time_group_delay(obs + idx_base, i, nav);

                // PRC = geometric_range - pseudorange
                obs[idx_rover].P[i] += geometric_range - pseudorange;
            }
        } else { // no matching base observations, invalidate rover pseudorange observations
            for(i = 0; i < opt->nf; i++) {
                obs[idx_rover].P[i] = 0.;
            }
        }
    }

    trace(4, "obs out=\n"); traceobs(4, obs, num_obs_total);

    free(rs); free(dts); free(var); free(azel);
}


/*
 * apply_dgnss_corrections
 *
 * Apply DGNSS corrections from base observations ('base_obs') to rover
 * observations ('rover_obs')
 */
int apply_dgnss_corrections(const prcopt_t* opt, const gtime_t* epoch_time, obs_t* rover_obs, const obs_t* base_obs, const nav_t* nav) {
    int i;
    obsd_t obs[MAXOBS*2];

    if(rover_obs->n == 0) {
        showmsg("No rover observations at t=%s: %i rover, %i base\n", time_str(*epoch_time, 3), rover_obs->n, base_obs->n);
        return -1;
    }

    if(base_obs->n == 0) {
        showmsg("No base observations at t=%s: %i rover, %i base\n", time_str(*epoch_time, 3), rover_obs->n, base_obs->n);
        return -1;
    }

    if(rover_obs->n + base_obs->n > MAXOBS*2) {
        showmsg("Too many observations at t=%s: %i rover, %i base\n", time_str(*epoch_time, 3), rover_obs->n, base_obs->n);
        return -1;
    }

    // copy in rover obs
    memcpy(obs, rover_obs->data, rover_obs->n * sizeof(obsd_t));
    for(i = 0; i < rover_obs->n; i++) {
        obs[i].rcv = 1;
    }

    // copy in base obs
    memcpy(&obs[rover_obs->n], base_obs->data, base_obs->n * sizeof(obsd_t));
    for(i = 0; i < base_obs->n; i++) {
        obs[i + rover_obs->n].rcv = 2;
    }

    // adjust pseudoranges
    adjust_pseudoranges(opt, obs, rover_obs->n, base_obs->n, nav);

    // copy out rover obs
    memcpy(rover_obs->data, obs, rover_obs->n * sizeof(obsd_t));

    return 0;
}


/* write rtcm3 msm to file -------------------------------------------------*/
static void write_rtcm3_msm(FILE* fp, rtcm_t* out, int msg, int sys, int sync) {
    obsd_t* data;
    obsd_t buff[MAXOBS];
    int i,j,n,ns,nobs,code,nsat=0,nsig=0,nmsg,mask[MAXCODE]={0};

    data = out->obs.data;
    nobs = out->obs.n;

    // count number of satellites and signals
    for(i = 0; i < nobs && i < MAXOBS; i++) {
        if(satsys(data[i].sat, NULL) != sys) {
            continue;
        }
        nsat++;
        for(j = 0; j < NFREQ+NEXOBS; j++) {
            if(!(code=data[i].code[j])||mask[code-1]) {
                continue;
            }
            mask[code-1] = 1;
            nsig++;
        }
    }
    if(nsig > 64) {
        return;
    }

    // pack data into multiple messages if nsat x nsig > 64
    if(nsig > 0) {
        ns = 64 / nsig;         /* max number of sats in a message */
        nmsg = (nsat-1) / ns+1; /* number of messages */
    } else {
        ns = 0;
        nmsg = 1;
    }
    out->obs.data = buff;

    for(i=j=0; i < nmsg; i++) {
        for(n=0; n < ns && j < nobs && j < MAXOBS; j++) {
            if(satsys(data[j].sat, NULL) != sys) {
                continue;
            }
            out->obs.data[n++] = data[j];
        }
        out->obs.n = n;

        if(gen_rtcm3(out, msg, 0, i < nmsg-1? 1: sync)) {
            fwrite(out->buff, out->nbyte, 1, fp);
        }
    }

    out->obs.data = data;
    out->obs.n = nobs;
}


static void write_rtcm3_obs(prcopt_t* popt, FILE* fp, const rtcm_t* rtcm_in, rtcm_t* rtcm_out) {
    int i, prn, sys;
    int navsys = popt->navsys;

    // copy observation data from 'rtcm_in' to 'rtcm_out'
    rtcm_out->time = rtcm_in->time;

    for(i = 0; i < rtcm_in->obs.n; i++) {
        rtcm_out->obs.data[i] = rtcm_in->obs.data[i];

        sys = satsys(rtcm_in->obs.data[i].sat, &prn);
        if(sys==SYS_GLO && rtcm_in->nav.glo_fcn[prn-1]) {
            rtcm_out->nav.glo_fcn[prn-1] = rtcm_in->nav.glo_fcn[prn-1];
        }
    }
    rtcm_out->obs.n = rtcm_in->obs.n;

    // generate and send MSM5 messages for enabled constellations
    if(navsys & SYS_GPS) {
        navsys &= ~SYS_GPS;
        write_rtcm3_msm(fp, rtcm_out, 1075, SYS_GPS, !navsys? 0: 1);
    }
    if(navsys & SYS_GLO) {
        navsys &= ~SYS_GLO;
        write_rtcm3_msm(fp, rtcm_out, 1085, SYS_GLO, !navsys? 0: 1);
    }
    if(navsys & SYS_GAL) {
        navsys &= ~SYS_GAL;
        write_rtcm3_msm(fp, rtcm_out, 1095, SYS_GAL, !navsys? 0: 1);
    }
    if(navsys & SYS_QZS) {
        navsys &= ~SYS_QZS;
        write_rtcm3_msm(fp, rtcm_out, 1115, SYS_QZS, !navsys? 0: 1);
    }
    if(navsys & SYS_CMP) {
        navsys &= ~SYS_CMP;
        write_rtcm3_msm(fp, rtcm_out, 1125, SYS_CMP, !navsys? 0: 1);
    }
}


static void write_rtcm3_nav(prcopt_t* popt, FILE* fp, const rtcm_t* rtcm_in, rtcm_t* rtcm_out) {
    int prn;
    int msg_type = 0;

    int sat = rtcm_in->ephsat;
    int set = rtcm_in->ephset;
    int sys = satsys(sat, &prn);

    if(!(popt->navsys & sys)) {
        return;
    }

    // copy navigation data from 'rtcm_in' to 'rtcm_out'
    rtcm_out->time = rtcm_in->time;

    if(sys == SYS_GLO) {
        rtcm_out->nav.geph[prn-1] = rtcm_in->nav.geph[prn-1];
        rtcm_out->ephsat = sat;
        rtcm_out->ephset = set;
    } else if(sys == SYS_GPS || sys == SYS_GAL || sys == SYS_QZS || sys == SYS_CMP || sys == SYS_IRN) {
        rtcm_out->nav.eph[sat-1+MAXSAT*set] = rtcm_in->nav.eph[sat-1+MAXSAT*set];
        rtcm_out->ephsat = sat;
        rtcm_out->ephset = set;
    }

    // generate and send ephemeris messages for enabled constellations
    switch(sys) {
        case SYS_GPS:
            msg_type = 1019;
            break;
        case SYS_GLO:
            msg_type = 1020;
            break;
        case SYS_GAL:
            msg_type = !set? 1046: 1045;
            break;
        case SYS_QZS:
            msg_type = 1044;
            break;
        case SYS_CMP:
            msg_type = 1042;
            break;
        default:
            return;
    }

    if(gen_rtcm3(rtcm_out, msg_type, 0, 0)) {
        fwrite(rtcm_out->buff, rtcm_out->nbyte, 1, fp);
    }
}


// read observations for 'epoch_time' from 'fp'
int get_obs_at_time(FILE* fp, rtcm_t* rtcm, const gtime_t* epoch_time, const char* filename) {
    long orig_pos;
    int msg_type;
    double tt;
    int ret = -1;

    while(1) {
        orig_pos = ftell(fp);

        msg_type = input_rtcm3f(rtcm, fp);

        if(msg_type == -2) {
            break;
        }

        if(msg_type == -1) {
            showmsg("RTCM parse error at byte %i of %s\n", ftell(fp), filename);
            continue;
        }

        if(msg_type != 1) { // only interested in observation messages
            continue;
        }

        tt = timediff(rtcm->time, *epoch_time);
        if(fabs(tt) <= DTTOL) { // same time
            ret = 1;
            break;
        } else if(tt > 0.0) {
            // observations are at a later time than the time of
            // interest, so rewind the stream
            fseek(fp, orig_pos, SEEK_SET);
            ret = 0;
            break;
        }
    }

    trace(3, "get_obs_at_time %s, ret %i\n", time_str(*epoch_time, 3), ret);
    return ret;
}


static int execses(prcopt_t* popt, const solopt_t* sopt, const filopt_t* fopt, const gtime_t* start_time, char** infile, int n, char* outfile) {
    FILE* fp_rover;
    FILE* fp_base;
    FILE* fp_out;
    prcopt_t popt_ = *popt;
    char tracefile[1024];
    int i, finished = 0;

    rtcm_t rtcm_rover, rtcm_base, rtcm_out;

    trace(3, "execses : n=%d outfile=%s\n", n, outfile);

    // open debug trace
    if(sopt->trace > 0) {
        if (*outfile) {
            strcpy(tracefile, outfile);
            strcat(tracefile, ".trace");
        }
        else {
            strcpy(tracefile, fopt->trace);
        }
        traceclose();
        traceopen(tracefile);
        tracelevel(sopt->trace);
    }

    init_rtcm(&rtcm_rover);
    rtcm_rover.time = *start_time;
    init_rtcm(&rtcm_base);
    rtcm_base.time = *start_time;
    init_rtcm(&rtcm_out);
    rtcm_out.time = *start_time;

    if(!(fp_rover = fopen(infile[0], "rb"))) {
         showmsg("Failed to open input file %s\n", infile[0]);
         return -1;
    }

    if(!(fp_base = fopen(infile[1], "rb"))) {
         showmsg("Failed to open input file %s\n", infile[1]);
         return -1;
    }

    if(!(fp_out = fopen(outfile, "wb"))) {
         showmsg("Failed to open output file %s\n", outfile);
         return -1;
    }

    while(!finished) {
        switch(input_rtcm3f(&rtcm_rover, fp_rover)) {
            case -2:
                finished = 1;
                break;
            case -1:
                showmsg("RTCM parse error at byte %i of %s\n", ftell(fp_rover), infile[0]);
                break;
            case 1: // observations
                showmsg("Processing t=%s", time_str(rtcm_rover.time, 3));

                if(get_obs_at_time(fp_base, &rtcm_base, &rtcm_rover.time, infile[1]) == 1) {
                    if(norm(rtcm_base.sta.pos, 3) > 0.) {
                        // copy base station location
                        for(i = 0; i < 3; i++) {
                            popt_.rb[i] = rtcm_base.sta.pos[i];
                        }

                        sortobs(&rtcm_rover.obs);
                        sortobs(&rtcm_base.obs);
                        apply_dgnss_corrections(&popt_, &rtcm_rover.time, &rtcm_rover.obs, &rtcm_base.obs, &rtcm_rover.nav);

                        // write out adjusted pseudoranegs
                        write_rtcm3_obs(&popt_, fp_out, &rtcm_rover, &rtcm_out);
                    } else {
                        showmsg("No base station position at t=%s\n", time_str(rtcm_rover.time, 3));
                    }
                } else {
                    showmsg("No corrections found for t=%s\n", time_str(rtcm_rover.time, 3));
                }
                break;
            case 2: // nav message
                write_rtcm3_nav(&popt_, fp_out, &rtcm_rover, &rtcm_out);
                break;
        }
    }

    fclose(fp_rover);
    fclose(fp_base);
    fclose(fp_out);

    free_rtcm(&rtcm_rover);
    free_rtcm(&rtcm_base);
    free_rtcm(&rtcm_out);

    /* close debug trace */
    traceclose();

    return 0;
}


int main(int argc, char* argv[]) {
    prcopt_t prcopt = prcopt_default;
    solopt_t solopt = solopt_default;
    filopt_t filopt = {""};
    int i, n, ret;
    char* infile[2];
    char* outfile = NULL;
    char* p = NULL;
    gtime_t trtcm = {0}; /* approx start time for rtcm */
    double epr[]={2010,1,1,0,0,0};

    prcopt.mode = PMODE_SINGLE; // disable usage of iono/tropo modelling
    prcopt.navsys = SYS_GPS | SYS_GAL | SYS_CMP;
    prcopt.refpos = 1;
    prcopt.glomodear = 1;
    prcopt.nf = 3;
    prcopt.elmin = 10. * D2R;

    solopt.timef = 0;
    sprintf(solopt.prog, "%s ver.%s %s", PROGNAME, VER_RTKLIB, PATCH_LEVEL);
    sprintf(filopt.trace,"%s.trace", PROGNAME);

    for(i=1, n=0; i < argc; i++) {
        if(!strcmp(argv[i], "-o") && i+1 <argc) {
            outfile = argv[++i];
        } else if(!strcmp(argv[i], "-f") && i+1 < argc) {
            prcopt.nf = atoi(argv[++i]);
        } else if(!strcmp(argv[i], "-sys") && i+1 < argc) {
            for(p = argv[++i]; *p; p++) {
                switch (*p) {
                    case 'G': prcopt.navsys|=SYS_GPS; break;
                    case 'R': prcopt.navsys|=SYS_GLO; break;
                    case 'E': prcopt.navsys|=SYS_GAL; break;
                    case 'J': prcopt.navsys|=SYS_QZS; break;
                    case 'C': prcopt.navsys|=SYS_CMP; break;
                    case 'I': prcopt.navsys|=SYS_IRN; break;
                }
                if(!(p=strchr(p,','))) break;
            }
        } else if(!strcmp(argv[i], "-x") && i+1 <argc) {
            solopt.trace = atoi(argv[++i]);
        } else if(!strcmp(argv[i], "-tr") && i+2 < argc) {
            sscanf(argv[++i], "%lf/%lf/%lf", epr, epr+1, epr+2);
            sscanf(argv[++i], "%lf:%lf:%lf", epr+3, epr+4, epr+5);
            trtcm = epoch2time(epr);
        } else if(*argv[i] == '-') {
            printhelp();
        } else {
            if(n == 2) {
                showmsg("error : too many input files\n");
                return -1;
            }

            infile[n++] = argv[i];
        }
    }
    if(n != 2) {
        showmsg("error : missing input files\n");
        return -2;
    }
    if(outfile == NULL) {
        showmsg("error : missing output file\n");
        return -3;
    }

    ret = execses(&prcopt, &solopt, &filopt, &trtcm, infile, n, outfile);

    if (!ret) fprintf(stderr,"%40s\r","");
    return ret;
}
