#include "USBDeviceList.h"
#include "USB.h"
#include "util.h"
#include "libusbjs.h"
#include "USBDeviceListIterator.h"

namespace libusbjs {

Napi::FunctionReference USBDeviceList::constructor;

void USBDeviceList::Init(Napi::Env env) {
  Napi::HandleScope scope(env);

  auto&& clazz = DefineClass(env, "USBDeviceList", {
    InstanceMethod(Napi::Symbol::WellKnown(env, "iterator"), &USBDeviceList::Iterator),
    InstanceAccessor("length", &USBDeviceList::Length, nullptr)
  });

  constructor = Napi::Persistent(clazz);
  constructor.SuppressDestruct();
}

USBDeviceList::USBDeviceList(const Napi::CallbackInfo& info) : Napi::ObjectWrap<USBDeviceList>(info) {
  auto&& usb = USB::Unwrap(info[0].ToObject());
  if (!usb) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "first param could not be unwrapped to USB class");
    return;
  }

  usb_ = Napi::Persistent(info[0].ToObject());
  
  auto&& errOrNum = libusb_get_device_list(usb->GetContext(), &devices_);
  if (failed_and_throw_js(info.Env(), errOrNum)) {
    return;
  }

  device_count_ = errOrNum;
}

USBDeviceList::~USBDeviceList() {
  if (devices_) {
    libusb_free_device_list(devices_, 0);
    devices_ = nullptr;
  }
}

Napi::Value USBDeviceList::Length(const Napi::CallbackInfo& info) {
  return Napi::Number::New(info.Env(), device_count_);
}

Napi::Value USBDeviceList::Iterator(const Napi::CallbackInfo& info) {
  return USBDeviceListIterator::constructor.New({Value()});
}

}