#include <napi.h>

#include "USB.h"
#include "USBDevice.h"
#include "USBDeviceHandle.h"
#include "USBDeviceList.h"
#include "USBDeviceListIterator.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    using namespace libusbjs;

    USB::Init(env);
    USBDevice::Init(env);
    USBDeviceHandle::Init(env);
    USBDeviceList::Init(env);
    USBDeviceListIterator::Init(env);
    

    exports.Set("USB", USB::constructor.Value());

    return exports;
}

NODE_API_MODULE(libusbjs, Init)