#ifndef libusbjs_USB_H_
#define libusbjs_USB_H_

#include <napi.h>
#include <libusb.h>

namespace libusbjs {

class USB : public Napi::ObjectWrap<USB> {
public:
  static void Init(Napi::Env env);
  static Napi::FunctionReference constructor;

public:
  USB(const Napi::CallbackInfo& info);
  ~USB();

public:
  Napi::Value Version(const Napi::CallbackInfo& info);
  void SetLogLevel(const Napi::CallbackInfo& info);
  void SetUseUSBDk(const Napi::CallbackInfo& info);
  Napi::Value GetDeviceList(const Napi::CallbackInfo& info);
  Napi::Value OpenDeviceWithVidPid(const Napi::CallbackInfo& info);

public:
  libusb_context* GetContext() { return context_; }

private:
  libusb_context* context_;
};

}

#endif