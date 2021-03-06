#ifndef __FRACTIZER_H
#define __FRACTIZER_H

#include <math.h>
#include <stdint.h>
#include <iostream>

#define INLINEIT static inline

typedef struct cp_t {
 double r;
 double i;
} cp_t;

typedef void (*znp1_calc_t)(cp_t *z, cp_t *c);

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
    uint8_t  do_julia;
    double   jx;
    double   jy;
    znp1_calc_t algo;
} fparams_t;


void showParams(fparams_t *p);
fparams_t make_default_params();
void generate_fractal(fparams_t *pparams, uint16_t *rbuf);

#endif

