#include "fractizer.h"
#include <napi.h>

class aWorker : public Napi::AsyncWorker {

public:
  aWorker(const Napi::Function& callback) : Napi::AsyncWorker(callback), bufptr(0) { }

protected:
  void Execute() override {
    std::cout << "Execute()" << std::endl;
    showParams(&parms);
    if (bufptr) {
        generate_fractal(&parms, bufptr);
        return;
    }
    std::cout << "no buffer" << std::endl;
  }

  void OnOK() override {
    std::cout << "OnOK()" << std::endl;
    Napi::Env env = Env();

    size_t len = parms.x_pels * parms.y_pels;
    Napi::Array oary = Napi::Array::New(env, len);
    for (uint32_t i=0;i<len;i++) {
        oary[i] = bufptr[i];
    }
    Callback().MakeCallback(
      Receiver().Value(), {
        env.Null(), oary
      }
    );
  }

public:
    bool get_named(Napi::Object parms_arg, const char *name, uint32_t &tuint, double &tdouble) {
        bool hasit = parms_arg.Has(name);
        if (hasit) {
            Napi::Value v = parms_arg.Get(name);
            tuint   = v.As<Napi::Number>().Uint32Value();
            tdouble = v.As<Napi::Number>().DoubleValue();
        }
        return hasit;
    };
    void unpack_params(Napi::Object parms_arg) {
        std::cout << "unpackParams()" << std::endl;
        set_default_params(&parms);

        uint32_t tuint;
        double tdouble;

        if (get_named(parms_arg, "max_iters", tuint, tdouble)) { parms.max_iters = tuint; };
        if (get_named(parms_arg, "escape_val", tuint, tdouble)) { parms.escape_val = tdouble; };
        if (get_named(parms_arg, "x_min", tuint, tdouble)) { parms.x_min = tdouble; };
        if (get_named(parms_arg, "x_max", tuint, tdouble)) { parms.x_max = tdouble; };
        if (get_named(parms_arg, "y_min", tuint, tdouble)) { parms.y_min = tdouble; };
        if (get_named(parms_arg, "y_max", tuint, tdouble)) { parms.y_max = tdouble; };
        if (get_named(parms_arg, "x_pels", tuint, tdouble)) { parms.x_pels = tuint; };
        if (get_named(parms_arg, "y_pels", tuint, tdouble)) { parms.y_pels= tuint; };
        if (get_named(parms_arg, "x_tile", tuint, tdouble)) { parms.x_tile = tuint; };
        if (get_named(parms_arg, "y_tile", tuint, tdouble)) { parms.y_tile = tuint; };
        if (get_named(parms_arg, "type", tuint, tdouble)) { parms.type = tuint; };
        if (get_named(parms_arg, "jx", tuint, tdouble)) { parms.jx = tdouble; };
        if (get_named(parms_arg, "jy", tuint, tdouble)) { parms.jy = tdouble; };

    };
    void setupBuffer() {
      std::cout << "setupBuffer()" << std::endl;
      size_t len = parms.x_pels * parms.y_pels;
      bufptr = new uint16_t[len];
    };


private:
    fparams_t parms;
    Napi::ArrayBuffer abuf;
    Napi::TypedArray  tary;
    uint16_t *bufptr;
};


void aRun(const Napi::CallbackInfo& info) {
  // Napi::Env env = info.Env();

  Napi::Object parms_arg = info[0].ToObject();
  Napi::Function cb = info[1].As<Napi::Function>();

  // std::cout << "info length:" << info.Length() << std::endl;
  // std::cout << "info 0 type :" << info[0].Type() << std::endl;
  // std::cout << "info 1 type :" << info[1].Type() << std::endl;

  auto w = new aWorker(cb);
  w->unpack_params(parms_arg); 
  w->setupBuffer();
  w->Queue();

  return;
}


Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "aRun"),
    Napi::Function::New(env, aRun)
  );
  return exports;
}


NODE_API_MODULE(addon, Init)
