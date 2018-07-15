{
    "targets": [{
        "target_name": "libusbjs",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
        "sources": [
            "./src/module.cc",
            "./src/util.cc",
            "./src/libusbjs.cc",
            "./src/USB.cc",
            "./src/USBDevice.cc",
            "./src/USBDeviceHandle.cc",
            "./src/USBDeviceList.cc",
            "./src/USBDeviceListIterator.cc"
        ],
        "include_dirs": [
            "./node_modules/node-addon-api",
            "./libusb/libusb"
        ],
        "dependencies": [
            "libusb.gypi:libusb"
        ],
    }]
}