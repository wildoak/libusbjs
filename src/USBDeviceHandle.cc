#include "USBDeviceHandle.h"
#include "USBDevice.h"
#include "util.h"
#include "libusbjs.h"

namespace libusbjs {


Napi::FunctionReference USBDeviceHandle::constructor;

void USBDeviceHandle::Init(Napi::Env env) {
  Napi::HandleScope scope(env);

  auto&& clazz = DefineClass(env, "USBDeviceHandle", {
    InstanceMethod("getDevice", &USBDeviceHandle::GetDevice),
    InstanceMethod("getConfiguration", &USBDeviceHandle::GetConfiguration),
    InstanceMethod("setConfiguration", &USBDeviceHandle::SetConfiguration),
    InstanceMethod("claimInterface", &USBDeviceHandle::ClaimInterface),
    InstanceMethod("releaseInterface", &USBDeviceHandle::ReleaseInterface),
    InstanceMethod("isKernelDriverActive", &USBDeviceHandle::IsKernelDriverActive),
    InstanceMethod("detachKernelDriver", &USBDeviceHandle::DetachKernelDriver),
    InstanceMethod("clearHalt", &USBDeviceHandle::ClearHalt),
    InstanceMethod("resetDevice", &USBDeviceHandle::ResetDevice),
    InstanceMethod("setAutoDetachKernelDriver", &USBDeviceHandle::SetAutoDetachKernelDriver),
    InstanceMethod("allocStreams", &USBDeviceHandle::AllocStreams),
    InstanceMethod("freeStreams", &USBDeviceHandle::FreeStreams)
  });

  constructor = Napi::Persistent(clazz);
  constructor.SuppressDestruct();
}

USBDeviceHandle::USBDeviceHandle(const Napi::CallbackInfo& info) : Napi::ObjectWrap<USBDeviceHandle>(info)
{

  if (!info[0].IsExternal()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "expect first argument to be external");
    return;
  }

  handle_ = Napi::Persistent(info[0].As<Napi::External<libusb_device_handle>>());
}

USBDeviceHandle::~USBDeviceHandle() {
  
}

Napi::Value USBDeviceHandle::GetDevice(const Napi::CallbackInfo& info) {
  auto&& device = libusb_get_device(handle_.Value().Data());
  if (!device) {
    throw_js(info.Env(), LIBUSBJS_INTERNAL_ERROR, "could not get device from handle");
    return info.Env().Undefined();
  }

  return USBDevice::constructor.New({
    Napi::External<libusb_device>::New(info.Env(), device)
  });
}

Napi::Value USBDeviceHandle::GetConfiguration(const Napi::CallbackInfo& info) {
  int config = -1;
  auto&& err = libusb_get_configuration(handle_.Value().Data(), &config);
  if (failed_and_throw_js(info.Env(), err)) {
    return info.Env().Undefined();
  }

  return Napi::Number::New(info.Env(), config);
}

void USBDeviceHandle::SetConfiguration(const Napi::CallbackInfo& info) {
  
  if (!info[0].IsNumber()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "expected number as first parameter");
    return;
  }

  auto&& err = libusb_set_configuration(handle_.Value().Data(), info[0].ToNumber().Int32Value());
  if (failed_and_throw_js(info.Env(), err)) {
    return;
  }
}


void USBDeviceHandle::ClaimInterface(const Napi::CallbackInfo& info) {
  if (!info[0].IsNumber()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "expected number as first parameter");
    return;
  }

  auto&& err = libusb_claim_interface(handle_.Value().Data(), info[0].ToNumber().Int32Value());
  if (failed_and_throw_js(info.Env(), err)) {
    return;
  }

}

void USBDeviceHandle::ReleaseInterface(const Napi::CallbackInfo& info) {
  if (!info[0].IsNumber()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "expected number as first parameter");
    return;
  }
  
  // TODO: release is blocking
  auto&& err = libusb_release_interface(handle_.Value().Data(), info[0].ToNumber().Int32Value());
  if (failed_and_throw_js(info.Env(), err)) {
    return;
  }
}

Napi::Value USBDeviceHandle::IsKernelDriverActive(const Napi::CallbackInfo& info) {
  if (!info[0].IsNumber()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "expected number as first parameter");
    return info.Env().Undefined();
  }

  auto&& errOrNumber = libusb_kernel_driver_active(handle_.Value().Data(), info[0].ToNumber().Int32Value());
  if (failed_and_throw_js(info.Env(), errOrNumber)) {
    return info.Env().Undefined();
  }

  // 1 means active, 0 means unactive
  return Napi::Boolean::New(info.Env(), errOrNumber == 1);
}

void USBDeviceHandle::DetachKernelDriver(const Napi::CallbackInfo& info) {
  if (!info[0].IsNumber()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "expected number as first parameter");
    return;
  }

  auto&& err = libusb_detach_kernel_driver(handle_.Value().Data(), info[0].ToNumber().Int32Value());
  if (failed_and_throw_js(info.Env(), err)) {
    return;
  }
}

void USBDeviceHandle::ClearHalt(const Napi::CallbackInfo& info) {
  if (!info[0].IsNumber()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "expected number as first parameter");
  }

  auto&& err = libusb_clear_halt(handle_.Value().Data(), info[0].ToNumber().Int32Value());
  if (failed_and_throw_js(info.Env(), err)) {
    return;
  }
}

void USBDeviceHandle::ResetDevice(const Napi::CallbackInfo& info) {
  auto&& err = libusb_reset_device(handle_.Value().Data());
  if (failed_and_throw_js(info.Env(), err)) {
    return;
  }
}

void USBDeviceHandle::SetAutoDetachKernelDriver(const Napi::CallbackInfo& info) {
  int enable = 1;
  if (info.Length() >= 1) {
    enable = info[0].ToBoolean().Value() ? 1 : 0;
  }

  auto&& err = libusb_set_auto_detach_kernel_driver(handle_.Value().Data(), enable);
  if (failed_and_throw_js(info.Env(), err)) {
    return;
  }
}

Napi::Value USBDeviceHandle::AllocStreams(const Napi::CallbackInfo& info) {
  if (!info[0].IsNumber()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "expected number as first parameter");
    return info.Env().Undefined();
  }

  if (!info[1].IsArray()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "expected array as second parameter");
    return info.Env().Undefined();
  }

  auto&& js_endpoints = info[1].ToObject();
  int num_endpoints = js_endpoints.Get("length").ToNumber().Int32Value();
  std::vector<unsigned char> endpoints(num_endpoints);
  for (int i = 0; i < num_endpoints; i++) {
    endpoints[i] = js_endpoints.Get(i).ToNumber().Uint32Value();
  }

  auto &&errOrNum = libusb_alloc_streams(handle_.Value().Data(), info[0].ToNumber().Uint32Value(), endpoints.data(), num_endpoints);
  if (failed_and_throw_js(info.Env(), errOrNum)) {
    return info.Env().Undefined();
  }

  return Napi::Number::New(info.Env(), errOrNum);
}

void USBDeviceHandle::FreeStreams(const Napi::CallbackInfo& info) {
  if (!info[0].IsArray()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "expected array as first parameter");
    return;
  }

  auto&& js_endpoints = info[0].ToObject();
  int num_endpoints = js_endpoints.Get("length").ToNumber().Int32Value();
  std::vector<unsigned char> endpoints(num_endpoints);
  for (int i = 0; i < num_endpoints; i++) {
    endpoints[i] = js_endpoints.Get(i).ToNumber().Uint32Value();
  }

  auto &&errOrNum = libusb_free_streams(handle_.Value().Data(), endpoints.data(), num_endpoints);
  if (failed_and_throw_js(info.Env(), errOrNum)) {
    return;
  }
}

}