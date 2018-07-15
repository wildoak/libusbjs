#ifndef libusbjs_USBDEVICEHANDLE_H_
#define libusbjs_USBDEVICEHANDLE_H_

#include <napi.h>
#include <libusb.h>

namespace libusbjs {

class USBDeviceHandle : public Napi::ObjectWrap<USBDeviceHandle> {
public:
  static void Init(Napi::Env env);
  static Napi::FunctionReference constructor;

public:
  USBDeviceHandle(const Napi::CallbackInfo& info);
  ~USBDeviceHandle();

  Napi::Value GetDevice(const Napi::CallbackInfo& info);
  Napi::Value GetConfiguration(const Napi::CallbackInfo& info);
  void SetConfiguration(const Napi::CallbackInfo& info);
  void ClaimInterface(const Napi::CallbackInfo& info);
  void ReleaseInterface(const Napi::CallbackInfo& info);
  Napi::Value IsKernelDriverActive(const Napi::CallbackInfo& info);
  void DetachKernelDriver(const Napi::CallbackInfo& info);
  void ClearHalt(const Napi::CallbackInfo& info);
  void ResetDevice(const Napi::CallbackInfo& info);
  void SetAutoDetachKernelDriver(const Napi::CallbackInfo& info);
  Napi::Value AllocStreams(const Napi::CallbackInfo& info);
  void FreeStreams(const Napi::CallbackInfo& info);

private:
  Napi::Reference<Napi::External<libusb_device_handle> > handle_;
};

}

#endif