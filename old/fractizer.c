#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

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


// typedef uint8_t (*ppxlcalc_t)(fparams_t *, double, double, uint16_t *, double *);

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
    if (argc > 12) params.do_julia    = atol(argv[12]);
    if (argc > 13) params.jx          = atof(argv[13]);
    if (argc > 14) params.jy          = atof(argv[14]);
    return params;

};

INLINEIT double cmag(cp_t *z) {
    return sqrt(z->r * z->r + z->i * z->i);
}
INLINEIT cp_t csquare(cp_t *a) {
    cp_t r;
    r.r = a->r * a->r - a->i * a->i;
    r.i = 2 * a->r * a->i;
    return r;
}

INLINEIT cp_t ccube(cp_t *z) {
    cp_t r;
    r.r = z->r * z->r * z->r - 3 * z->r * z->i * z->i;
    r.i = 3 * z->r * z->r * z->i - z->i * z->i * z->i;
    return r;
};

INLINEIT cp_t cadd(cp_t *a, cp_t *b) {
    cp_t r;
    r.r = a->r + b->r;
    r.i = a->i + b->i;
    return r;
};

INLINEIT cp_t csmul(cp_t *a, double m) {
    cp_t r;
    r.r = a->r * m;
    r.i = a->i * m;
    return r;
}


INLINEIT uint8_t calc_pixel_generic(fparams_t *pparams, double x, double y, uint16_t *piters, double *pval) {
    cp_t z, c;
    if (pparams->do_julia) {
        z.r = x; z.i = y; c.r = pparams->jx; c.i = pparams->jy;
    } else {
        z.r = 0; z.i = 0; c.r = x; c.i = y;
    };
    uint16_t iter = 0;
    double val = 0;
    while ((iter < pparams->max_iters) && (val < pparams->escape_val)) {
        pparams->algo(&z, &c);
        val = cmag(&z);
        iter++;
    }
    *pval = val;
    *piters = iter;
    return iter < pparams->max_iters;
};

INLINEIT void znp1_mb(cp_t *z, cp_t *c) {
    cp_t nz;
    double n_z_r = z->r * z->r - z->i * z->i + c->r;
    nz.i = 2.0 * z->r * z->i + c->i;
    nz.r = n_z_r;
    *z = nz;
}

INLINEIT void znp1_cos(cp_t *z, cp_t *c) {
    cp_t nz;
    nz.r = cos(z->r) * cosh(z->i) + c->r;
    nz.i = -sin(z->r) * sinh(z->i) + c->i;
    *z = nz;
}

INLINEIT void znp1_mbtc(cp_t *z, cp_t *c) {
    z->i = -z->i;
    cp_t nz;
    nz.r = z->r * z->r - z->i * z->i + c->r;
    nz.i = 2.0 * z->r * z->i + c->i;
    *z = nz;;
}

INLINEIT void znp1_mbbs(cp_t *z, cp_t *c) {
    z->r = fabs(z->r);
    z->i = fabs(z->i);
    cp_t nz;
    nz.r = z->r * z->r - z->i * z->i + c->r;
    nz.i = 2.0 * z->r * z->i + c->i;
    *z = nz;;
}

INLINEIT void znp1_mb3(cp_t *z, cp_t *c) {
    cp_t nz;
    nz = ccube(z);
    nz.r += c->r;
    nz.i += c->i;
    *z = nz;;
}

INLINEIT void znp1_nf3(cp_t *z, cp_t *c) {
    cp_t nz = ccube(z);
    nz.r += c->r - 1.0;
    nz.i += c->i;
    *z = nz;
}

INLINEIT void znp1_nf3m2z(cp_t *z, cp_t *c) {
    cp_t a = ccube(z);
    cp_t b = csmul(z,-2);
    cp_t nz = cadd(&a,&b);
    nz.r += c->r;
    nz.i += c->i;
    *z = nz;
};

INLINEIT void znp1_nf8p15z4(cp_t *z, cp_t *c) {
    cp_t z2 = csquare(z);
    cp_t z4 = csquare(&z2);
    cp_t z8 = csquare(&z4);
    cp_t z14_15 = csmul(&z4,15);
    cp_t nz = cadd(&z8,&z14_15);
    nz.r += c->r - 0;
    nz.i += c->i;
    *z = nz;
}

void generate_fractal(fparams_t *pparams, uint16_t *rbuf) {
    double x_step = (pparams->x_max - pparams->x_min) / (double)pparams->x_pels;
    double y_step = (pparams->y_max - pparams->y_min) / (double)pparams->y_pels;

    pparams->algo = znp1_mb;

    switch (pparams->type) {
        case 1 : pparams->algo = znp1_mb3; break;
        case 2 : pparams->algo = znp1_mbbs; break;
        case 3 : pparams->algo = znp1_mbtc; break;
        case 4 : pparams->algo = znp1_cos; break;
        case 5 : pparams->algo = znp1_nf3; break;
        case 6 : pparams->algo = znp1_nf3m2z; break;
        case 7 : pparams->algo = znp1_nf8p15z4; break;
        default: pparams->algo = znp1_mb; break;
    }

    for (uint16_t j=0; j<pparams->y_pels; j++) {
        double y = pparams->y_min + j * y_step;
        for (uint16_t i=0; i<pparams->x_pels; i++) {
            double x = pparams->x_min + i * x_step;
            uint16_t iters = 0;
            double val = 0;
            calc_pixel_generic(pparams, x, y, &iters, &val);
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

