#include <math.h>
#include <stdint.h>
#include <node_api.h>
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

fparams_t make_default_params() {
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
    params.do_julia = 0;
    params.jx = 0;
    params.jy = 0;
    return params;
};


bool get_named(napi_env env, napi_value obj, const char *name, uint32_t *tuint, double *tdouble) {
    bool hasit = false;
    napi_has_named_property(env, obj, name, &hasit);
    if (hasit) {
        napi_value nobj;
        napi_get_named_property(env, obj, name, &nobj);
        napi_get_value_uint32(env, nobj, tuint);
        napi_get_value_double(env, nobj, tdouble);
    }
    return hasit;
};


fparams_t unpack_node_params2(napi_env env, napi_callback_info info) {
    fparams_t params = make_default_params();

    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    
    uint32_t tuint;
    double tdouble;
    if (get_named(env, argv[0], "max_iters", &tuint, &tdouble)) { params.max_iters = tuint; };
    if (get_named(env, argv[0], "escape_val", &tuint, &tdouble)) { params.escape_val = tdouble; };
    if (get_named(env, argv[0], "x_min", &tuint, &tdouble)) { params.x_min = tdouble; };
    if (get_named(env, argv[0], "x_max", &tuint, &tdouble)) { params.x_max = tdouble; };
    if (get_named(env, argv[0], "y_min", &tuint, &tdouble)) { params.y_min = tdouble; };
    if (get_named(env, argv[0], "y_max", &tuint, &tdouble)) { params.y_max = tdouble; };
    if (get_named(env, argv[0], "x_pels", &tuint, &tdouble)) { params.x_pels = tuint; };
    if (get_named(env, argv[0], "y_pels", &tuint, &tdouble)) { params.y_pels= tuint; };
    if (get_named(env, argv[0], "x_tile", &tuint, &tdouble)) { params.x_tile = tuint; };
    if (get_named(env, argv[0], "y_tile", &tuint, &tdouble)) { params.y_tile = tuint; };
    if (get_named(env, argv[0], "type", &tuint, &tdouble)) { params.type = tuint; };
    if (get_named(env, argv[0], "jx", &tuint, &tdouble)) { params.jx = tdouble; };
    if (get_named(env, argv[0], "jy", &tuint, &tdouble)) { params.jy = tdouble; };

    return params;

};

// typedef uint8_t (*ppxlcalc_t)(fparams_t *, double, double, uint16_t *, double *);
fparams_t unpack_node_params(napi_env env, napi_callback_info info) {
    fparams_t params = make_default_params();;
    
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    uint32_t alen = 0;
    napi_get_array_length(env, argv[0], &alen);

    for (uint8_t i=0;i<alen;i++) {
        napi_value elem;
        napi_get_element(env, argv[0], i, &elem);
        uint32_t tuint;
        double   tdouble;
        napi_get_value_uint32(env, elem, &tuint);
        napi_get_value_double(env, elem, &tdouble);
        switch (i) {
            case 0 : params.max_iters   = tuint; break;
            case 1 : params.escape_val  = tdouble; break;
            case 2 : params.x_min       = tdouble; break;
            case 3 : params.x_max       = tdouble; break;
            case 4 : params.y_min       = tdouble; break;
            case 5 : params.y_max       = tdouble;  break;
            case 6 : params.x_pels      = tuint; break;
            case 7 : params.y_pels      = tuint; break;
            case 8 : params.x_tile      = tuint; break;
            case 9 : params.y_tile      = tuint; break;
            case 10 : params.type        = tuint; break;
            case 11 : params.do_julia    = tuint; break;
            case 12 : params.jx          = tdouble; break;
            case 13 : params.jy          = tdouble; break;
            default : break;
        } 
    }
    return params;
}


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

napi_value runFunc(napi_env env, napi_callback_info info) {

    fparams_t params = unpack_node_params2(env, info);
    showParams(&params);

    void *vrdata;
    size_t len = params.x_pels * params.y_pels;
    napi_value abuf, oary;
    napi_create_arraybuffer(env, len * sizeof(uint16_t), &vrdata, &abuf);
    uint16_t *rdata = (uint16_t *)vrdata;
    napi_create_typedarray(env, napi_uint16_array, len, abuf, 0, &oary);

    if (rdata) {
        generate_fractal(&params, rdata);
        return oary;
    }
    napi_get_null(env, &oary);
    return oary;
}


napi_value Init(napi_env env, napi_value exports) {

    napi_status status;
    napi_value fn;
    status = napi_create_function(env, NULL, 0, runFunc, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env,NULL,"Unable to wrap native function.");
    }
    status = napi_set_named_property(env, exports, "run", fn);
    if (status != napi_ok) {
       napi_throw_error(env,NULL,"Unable to populate exports"); 
    }
    return exports;

};

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)

