#include "fractizer.h"

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

