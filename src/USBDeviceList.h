#ifndef libusbjs_USBDEVICELIST_H_
#define libusbjs_USBDEVICELIST_H_

#include <napi.h>
#include <libusb.h>

namespace libusbjs {

class USBDeviceList : public Napi::ObjectWrap<USBDeviceList> {
public:
  static void Init(Napi::Env env);
  static Napi::FunctionReference constructor;
  
public:
  USBDeviceList(const Napi::CallbackInfo& info);
  ~USBDeviceList();

public:
  Napi::Value Iterator(const Napi::CallbackInfo& info);
  Napi::Value Length(const Napi::CallbackInfo& info);

public:
  int GetDeviceCount() const { return device_count_; }
  libusb_device** GetDevices() { return devices_; }

private:
  Napi::ObjectReference usb_;
  libusb_device** devices_;
  int device_count_;
};

}

#endif