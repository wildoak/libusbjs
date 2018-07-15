#include "USBDevice.h"
#include "USBDeviceList.h"
#include "libusbjs.h"
#include "util.h"
#include "USBDeviceHandle.h"

namespace libusbjs {

Napi::FunctionReference USBDevice::constructor;

void USBDevice::Init(Napi::Env env) {
  auto&& clazz = DefineClass(env, "USBDevice", {
    InstanceMethod("getPortNumber", &USBDevice::GetPortNumber),
    InstanceMethod("getPortNumbers", &USBDevice::GetPortNumbers),
    InstanceMethod("getParent", &USBDevice::GetParent),
    InstanceMethod("getDeviceAddress", &USBDevice::GetDeviceAddress),
    InstanceMethod("getDeviceSpeed", &USBDevice::GetDeviceSpeed),
    InstanceMethod("getMaxPacketSize", &USBDevice::GetMaxPacketSize),
    InstanceMethod("getMaxIsoPacketSize", &USBDevice::GetMaxIsoPacketSize)
  });

  constructor = Napi::Persistent(clazz);
  constructor.SuppressDestruct();
}

USBDevice::USBDevice(const Napi::CallbackInfo& info) :
  Napi::ObjectWrap<USBDevice>(info),
  device_(nullptr)
{
  Napi::HandleScope scope(info.Env());

  if (!info[0].IsExternal()) {
    throw_js(info.Env(), LIBUSBJS_INVALID_PARAM, "first param could not be external libusb_device");
    return;
  }
  
  auto&& device = info[0].As<Napi::External<libusb_device>>();
  device_ = libusb_ref_device(device.Data());
}

USBDevice::~USBDevice() {
  if (device_) {
    libusb_unref_device(device_);
    device_ = nullptr;
  }
}

Napi::Value USBDevice::GetPortNumber(const Napi::CallbackInfo& info) {
  auto&& port_number = libusb_get_port_number(device_);
  return Napi::Number::New(info.Env(), port_number);
}

Napi::Value USBDevice::GetPortNumbers(const Napi::CallbackInfo& info) {
  Napi::EscapableHandleScope scope(info.Env());

  // from the doc: As per the USB 3.0 specs, the current maximum limit for the depth is 7.
  constexpr int ports_size = 64;
  uint8_t ports[ports_size];
  auto&& errOrNumber = libusb_get_port_numbers(device_, ports, ports_size);
  if (failed_and_throw_js(info.Env(), errOrNumber)) {
    return info.Env().Undefined();
  }

  auto&& array = Napi::Uint32Array::New(info.Env(), errOrNumber);
  for (int i = 0; i < errOrNumber; i++) {
    array[i] = Napi::Number::New(info.Env(), ports[i]);
  }

  return scope.Escape(array);
}

Napi::Value USBDevice::GetParent(const Napi::CallbackInfo& info) {
  Napi::EscapableHandleScope scope(info.Env());
  
  auto&& parent = libusb_get_parent(device_);
  if (!parent) {
    return info.Env().Null();
  }

  auto&& js_parent = USBDevice::constructor.New({
    Napi::External<libusb_device>::New(info.Env(), parent)
  });

  return scope.Escape(js_parent);
}

Napi::Value USBDevice::GetDeviceAddress(const Napi::CallbackInfo& info) {
  auto&& device_address = libusb_get_device_address(device_);
  return Napi::Number::New(info.Env(), device_address);
}

Napi::Value USBDevice::GetDeviceSpeed(const Napi::CallbackInfo& info) {
  auto&& speed = libusb_get_device_speed(device_);
  return Napi::Number::New(info.Env(), speed);
}

Napi::Value USBDevice::GetMaxPacketSize(const Napi::CallbackInfo& info) {
  Napi::EscapableHandleScope scope(info.Env());

  unsigned char endpoint = 0;
  if (info.Length() >= 1) {
    endpoint = (unsigned char)info[0].ToNumber().Uint32Value();
  }

  auto&& errOrNumber = libusb_get_max_packet_size(device_, endpoint);
  if (failed_and_throw_js(info.Env(), errOrNumber)) {
    return info.Env().Undefined();
  }

  auto&& js_max_packet_size = Napi::Number::New(info.Env(), errOrNumber);
  return scope.Escape(js_max_packet_size);
}

Napi::Value USBDevice::GetMaxIsoPacketSize(const Napi::CallbackInfo& info) {
  Napi::EscapableHandleScope scope(info.Env());

  unsigned char endpoint = 0;
  if (info.Length() >= 1) {
    endpoint = (unsigned char)info[0].ToNumber().Uint32Value();
  }

  auto&& errOrNumber = libusb_get_max_iso_packet_size(device_, endpoint);
  if (failed_and_throw_js(info.Env(), errOrNumber)) {
    return info.Env().Undefined();
  }

  auto&& js_max_packet_size = Napi::Number::New(info.Env(), errOrNumber);
  return scope.Escape(js_max_packet_size);
}

Napi::Value USBDevice::Open(const Napi::CallbackInfo& info) {
  libusb_device_handle* handle = nullptr;
  auto&& err = libusb_open(GetDevice(), &handle);
  if (failed_and_throw_js(info.Env(), err)) {
    return info.Env().Undefined();
  }

  return USBDeviceHandle::constructor.New({
    Napi::External<libusb_device_handle>::New(info.Env(), handle, [](Napi::Env env, libusb_device_handle* handle) {
      libusb_close(handle);
    })
  });
}

}