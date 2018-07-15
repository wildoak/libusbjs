#include "USB.h"
#include "util.h"
#include "libusbjs.h"
#include "USBDeviceList.h"
#include "USBDeviceHandle.h"

namespace libusbjs {

Napi::FunctionReference USB::constructor;

void USB::Init(Napi::Env env) {
  Napi::HandleScope scope(env);

  auto&& clazz = DefineClass(env, "USB", {
    InstanceMethod("version", &USB::Version),
    InstanceMethod("setLogLevel", &USB::SetLogLevel),
    InstanceMethod("setUseUSBDk", &USB::SetUseUSBDk),
    InstanceMethod("getDeviceList", &USB::GetDeviceList),
    InstanceMethod("openDeviceWithVidPid", &USB::OpenDeviceWithVidPid)
  });

  clazz.Set("LIBUSB_LOG_LEVEL_DEBUG", Napi::Number::New(env, LIBUSB_LOG_LEVEL_DEBUG));
  clazz.Set("LIBUSB_LOG_LEVEL_INFO", Napi::Number::New(env, LIBUSB_LOG_LEVEL_INFO));
  clazz.Set("LIBUSB_LOG_LEVEL_WARNING", Napi::Number::New(env, LIBUSB_LOG_LEVEL_WARNING));
  clazz.Set("LIBUSB_LOG_LEVEL_ERROR", Napi::Number::New(env, LIBUSB_LOG_LEVEL_ERROR));
  clazz.Set("LIBUSB_LOG_LEVEL_NONE", Napi::Number::New(env, LIBUSB_LOG_LEVEL_NONE));

  constructor = Napi::Persistent(clazz);
  constructor.SuppressDestruct();
}

USB::USB(const Napi::CallbackInfo& info) : Napi::ObjectWrap<USB>(info) {
  int err = libusb_init(&context_);
  failed_and_throw_js(info.Env(), err);
}

USB::~USB() {
  libusb_exit(context_);
  context_ = nullptr;
}

Napi::Value USB::Version(const Napi::CallbackInfo& info) {
  Napi::EscapableHandleScope scope(info.Env());

  auto&& version = libusb_get_version();
  auto&& result = Napi::Object::New(info.Env());

  result.Set("describe", Napi::String::New(info.Env(), version->describe));
  result.Set("major", Napi::Number::New(info.Env(), version->major));
  result.Set("micro", Napi::Number::New(info.Env(), version->micro));
  result.Set("minor", Napi::Number::New(info.Env(), version->minor));
  result.Set("nano", Napi::Number::New(info.Env(), version->nano));
  result.Set("rc", Napi::String::New(info.Env(), version->rc));

  return scope.Escape(result);
}

void USB::SetLogLevel(const Napi::CallbackInfo& info) {
  Napi::HandleScope scope(info.Env());

  if (!info[0].IsNumber()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "expected number for logLevel");
    return;
  }

  int err = libusb_set_option(context_, LIBUSB_OPTION_LOG_LEVEL, info[0].ToNumber().Int32Value());
  failed_and_throw_js(info.Env(), err);
}

void USB::SetUseUSBDk(const Napi::CallbackInfo& info) {
  Napi::HandleScope scope(info.Env());

  int err = libusb_set_option(context_, LIBUSB_OPTION_USE_USBDK);
  failed_and_throw_js(info.Env(), err);
}

Napi::Value USB::GetDeviceList(const Napi::CallbackInfo& info) {
  Napi::EscapableHandleScope scope(info.Env());

  auto&& device_list = USBDeviceList::constructor.New({
    Value()
  });
  
  return scope.Escape(device_list);
}

Napi::Value USB::OpenDeviceWithVidPid(const Napi::CallbackInfo& info) {
  uint16_t vendor_id = info[0].ToNumber().Uint32Value();
  uint16_t product_id = info[1].ToNumber().Uint32Value();
  
  auto&& handle = libusb_open_device_with_vid_pid(context_, vendor_id, product_id);
  if (!handle) {
    throw_js(info.Env(), LIBUSBJS_INTERNAL_ERROR, std::string() + "could not open device by vendor_id " +
    std::to_string(vendor_id) + " and product_id " + std::to_string(product_id));
    return info.Env().Undefined();
  }

  return USBDeviceHandle::constructor.New({
    
    Napi::External<libusb_device_handle>::New(info.Env(), handle, [](Napi::Env env, libusb_device_handle* handle){
      libusb_close(handle);
    })
  });
}

}