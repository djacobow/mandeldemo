{
    "targets": [
        {
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            "dependencies": [
                "<!(node -p \"require('node-addon-api').gyp\")"
            ],
            "target_name": "fractizer",
            "sources": [ "native/fractizer.cpp", "native/wrapper.cpp" ],
            "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ]
        }
    ]
}
