#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

typedef struct fparams_t {
    uint16_t max_iters;
    double   escape_val;
    double   x_min;
    double   x_max;
    double   y_min;
    double   y_max;
    uint16_t x_pels;
    uint16_t y_pels;
    uint16_t x_tile;
    uint16_t y_tile;
    uint8_t  type;
} fparams_t;


fparams_t get_params(int argc, char *argv[]) {
    fparams_t params;
    params.max_iters = 200;
    params.escape_val = 4;
    params.x_min = -2;
    params.x_max = 2;
    params.y_min = -2;
    params.y_max = 2;
    params.x_pels = 1024;
    params.y_pels = 1024;
    params.x_tile = 0;
    params.y_tile = 0;
    params.type = 0;

    if (argc > 1)  params.max_iters   = atol(argv[1]);
    if (argc > 2)  params.escape_val  = atof(argv[2]);
    if (argc > 3)  params.x_min       = atof(argv[3]);
    if (argc > 4)  params.x_max       = atof(argv[4]);
    if (argc > 5)  params.y_min       = atof(argv[5]);
    if (argc > 6)  params.y_max       = atof(argv[6]);
    if (argc > 7)  params.x_pels      = atol(argv[7]);
    if (argc > 8)  params.y_pels      = atol(argv[8]);
    if (argc > 9)  params.x_tile      = atol(argv[9]);
    if (argc > 10) params.y_tile      = atol(argv[10]);
    if (argc > 11) params.type        = atol(argv[11]);
    return params;

};

/*
int open_socket(char *host, uint16_t port) {
    int sockfd = 0;
    int n =0;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        puts("Could not create socket.");
        return sockfd;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    int pt_res = inet_pton(AF_INET, host, &serv_addr.sin_addr);
    if (pt_res <= 0) {
        printf("HOST: %s pt_res %d\n",host, pt_res);
        puts("inet_pton error");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        puts("connect failed");
        return -1;
    }
    return sockfd;
};
*/


static inline uint8_t calc_pixel_mb(fparams_t *pparams, double x, double y, uint16_t *piters, double *pval) {
    double z_r = 0;
    double z_i = 0;
    double c_r = x;
    double c_i = y;
    uint16_t iter = 0;
    double val = 0;
    while ((iter < pparams->max_iters) && (val < pparams->escape_val)) {
        double z_n1_r = z_r * z_r - z_i * z_i + c_r;
        double z_n1_i = 2.0 * z_r * z_i + c_i;
        val = sqrt(z_n1_r * z_n1_r + z_n1_i * z_n1_i);
        iter++;
        z_r = z_n1_r;
        z_i = z_n1_i;
    }
    *pval = val;
    *piters = iter;
    return iter < pparams->max_iters;
};

static inline uint8_t calc_pixel_mbbs(fparams_t *pparams, double x, double y, uint16_t *piters, double *pval) {
    double z_r = 0;
    double z_i = 0;
    double c_r = x;
    double c_i = y;
    uint16_t iter = 0;
    double val = 0;
    while ((iter < pparams->max_iters) && (val < pparams->escape_val)) {
        z_r = fabs(z_r);
        z_i = fabs(z_i);
        double z_n1_r = z_r * z_r - z_i * z_i + c_r;
        double z_n1_i = 2.0 * z_r * z_i + c_i;
        val = sqrt(z_n1_r * z_n1_r + z_n1_i * z_n1_i);
        iter++;
        z_r = z_n1_r;
        z_i = z_n1_i;
    }
    *pval = val;
    *piters = iter;
    return iter < pparams->max_iters;
};

static inline uint8_t calc_pixel_mb3(fparams_t *pparams, double x, double y, uint16_t *piters, double *pval) {
    double z_r = 0;
    double z_i = 0;
    double c_r = x;
    double c_i = y;
    uint16_t iter = 0;
    double val = 0;
    while ((iter < pparams->max_iters) && (val < pparams->escape_val)) {
        double z_n1_r = z_r * z_r * z_r - 3 * z_r * z_i * z_i + c_r;
        double z_n1_i = 3 * z_r * z_r * z_i - z_i * z_i * z_i + c_i;
        val = sqrt(z_n1_r * z_n1_r + z_n1_i * z_n1_i);
        iter++;
        z_r = z_n1_r;
        z_i = z_n1_i;
    }
    *pval = val;
    *piters = iter;
    return iter < pparams->max_iters;
};

void generate_fractal(fparams_t *pparams, uint16_t *rbuf) {
    double x_step = (pparams->x_max - pparams->x_min) / (double)pparams->x_pels;
    double y_step = (pparams->y_max - pparams->y_min) / (double)pparams->y_pels;
    for (uint16_t j=0; j<pparams->y_pels; j++) {
        double y = pparams->y_min + j * y_step;
        for (uint16_t i=0; i<pparams->x_pels; i++) {
            double x = pparams->x_min + i * x_step;
            uint16_t iters = 0;
            double val = 0;
            switch (pparams->type) {
                case 1 : calc_pixel_mb3(pparams, x,y, &iters, &val); break;
                case 2 : calc_pixel_mbbs(pparams, x,y, &iters, &val); break;
                default: calc_pixel_mb(pparams, x,y, &iters, &val);
            }
            rbuf[i + j * pparams->x_pels] = iters;
        }
    };
};

void dump_fractal(fparams_t *pparams, uint16_t *rbuf) {
    printf("%u %f %f %f %f %f %u %u %u %u %u\n",
            pparams->max_iters,
            pparams->escape_val,
            pparams->x_min,
            pparams->x_max,
            pparams->y_min,
            pparams->y_max,
            pparams->x_pels,
            pparams->y_pels,
            pparams->x_tile,
            pparams->y_tile,
            pparams->type
            );
    uint32_t len = pparams->x_pels * pparams->y_pels;
    for (uint32_t i=0; i<len; i++) {
        printf("%u\n",rbuf[i]);
    }
    printf("all_done\n");
    fflush(stdout);
};

int main(int argc, char *argv[]) {
    fparams_t params = get_params(argc, argv);

    uint16_t *rdata = malloc(sizeof(uint16_t) * params.x_pels * params.y_pels);
    if (rdata) {
        generate_fractal(&params, rdata);
        if (1) {
            dump_fractal(&params, rdata);
        }
    }
};

