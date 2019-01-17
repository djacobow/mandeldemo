#include "fractizer.h"
#include <stdio.h>

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

void set_default_params(fparams_t *p) {
    p->max_iters = 200;
    p->escape_val = 4;
    p->x_min = -2;
    p->x_max = 2;
    p->y_min = -2;
    p->y_max = 2;
    p->x_pels = 1024;
    p->y_pels = 1024;
    p->x_tile = 0;
    p->y_tile = 0;
    p->type = 0;
    p->do_julia = 0;
    p->jx = 0;
    p->jy = 0;
};

void showParams(fparams_t *p) {
    printf("max_iters   : %d\n",p->max_iters);
    printf("escape_Val  : %f\n",p->escape_val);
    printf("x_range     : [%f : %f ]\n",p->x_min,p->x_max);
    printf("y_range     : [%f : %f ]\n",p->y_min,p->y_max);
    printf("size        : (%d : %d )\n",p->x_pels,p->y_pels);
    printf("tiles       : (%d : %d )\n",p->x_tile,p->y_tile);
    printf("type        : %d\n",p->type);
    printf("do_julia    : %d\n",p->do_julia);
    printf("jparams     : (%f : %f )\n",p->jx,p->jy);
};

/*
void showParams(fparams_t *p) {
    std::cout << "max_iters   : " << p->max_iters  << std::endl;
    std::cout << "escape_Val  : " << p->escape_val << std::endl;
    std::cout << "x_range     : " << "[" << p->x_min << ":" << p->x_max << "]" << std::endl;
    std::cout << "y_range     : " << "[" << p->y_min << ":" << p->y_max << "]" << std::endl;
    std::cout << "size        : " << "(" << p->x_pels << "," << p->y_pels<< ")" << std::endl;
    std::cout << "tiles       : " << "(" << p->x_tile << "," << p->y_tile << ")" << std::endl;
    std::cout << "type        : " << p->type << std::endl;
    std::cout << "do_julia    : " << p->do_julia << std::endl;
    std::cout << "jparams     : " << "(" << p->jx << "," << p->jy << ")" << std::endl;
};
*/
