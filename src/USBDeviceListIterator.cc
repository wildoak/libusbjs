#include "USBDeviceListIterator.h"
#include "USBDeviceList.h"
#include "USBDevice.h"
#include "libusbjs.h"
#include "util.h"

namespace libusbjs {

Napi::FunctionReference USBDeviceListIterator::constructor;

void USBDeviceListIterator::Init(Napi::Env env) {
  auto&& clazz = DefineClass(env, "USBDeviceListIterator", {
    InstanceMethod("next", &USBDeviceListIterator::Next)
  });

  constructor = Napi::Persistent(clazz);
  constructor.SuppressDestruct();
}

USBDeviceListIterator::USBDeviceListIterator(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<USBDeviceListIterator>(info), index_(0) {
  auto&& device_list = USBDeviceList::Unwrap(info[0].ToObject());
  if (!device_list) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "first param could not be unwrapped to USBDeviceList class");
    return;
  }

  device_list_ = Napi::Persistent(info[0].ToObject());
}

Napi::Value USBDeviceListIterator::Next(const Napi::CallbackInfo& info) {
  Napi::EscapableHandleScope scope(info.Env());
  auto&& device_list = USBDeviceList::Unwrap(device_list_.Value());
  
  Napi::Value done;
  Napi::Value value;

  if (index_ < device_list->GetDeviceCount()) {
    done = Napi::Boolean::New(info.Env(), false);
    value = USBDevice::constructor.New({
      Napi::External<libusb_device>::New(info.Env(), device_list->GetDevices()[index_])
    });

    index_++;
  } else {
    done = Napi::Boolean::New(info.Env(), true);
    value = info.Env().Undefined();
  }

  auto&& entry = Napi::Object::New(info.Env());
  entry.Set("done", done);
  entry.Set("value", value);

  return scope.Escape(entry);
}

}