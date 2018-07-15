#ifndef libusbjs_USBDEVICE_H_
#define libusbjs_USBDEVICE_H_

#include <napi.h>
#include <libusb.h>

namespace libusbjs {

class USBDevice : public Napi::ObjectWrap<USBDevice> {
public:
  static void Init(Napi::Env env);
  static Napi::FunctionReference constructor;

public:
  USBDevice(const Napi::CallbackInfo& info);
  ~USBDevice();

  Napi::Value GetPortNumber(const Napi::CallbackInfo& info);
  Napi::Value GetPortNumbers(const Napi::CallbackInfo& info);
  Napi::Value GetParent(const Napi::CallbackInfo& info);
  Napi::Value GetDeviceAddress(const Napi::CallbackInfo& info);
  Napi::Value GetDeviceSpeed(const Napi::CallbackInfo& info);
  Napi::Value GetMaxPacketSize(const Napi::CallbackInfo& info);
  Napi::Value GetMaxIsoPacketSize(const Napi::CallbackInfo& info);
  Napi::Value Open(const Napi::CallbackInfo& info);

  libusb_device* GetDevice() { return device_; }

private:
  libusb_device* device_;
};

}

#endif