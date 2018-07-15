#ifndef libusbjs_USBDEVICE_LIST_ITERATOR_H_
#define libusbjs_USBDEVICE_LIST_ITERATOR_H_

#include <napi.h>

namespace libusbjs {

class USBDeviceListIterator : public Napi::ObjectWrap<USBDeviceListIterator> {
public:
  static void Init(Napi::Env env);
  static Napi::FunctionReference constructor;

public:
  USBDeviceListIterator(const Napi::CallbackInfo& info);
  ~USBDeviceListIterator();

public:
  Napi::Value Next(const Napi::CallbackInfo& info);

private:
  Napi::ObjectReference device_list_;
  int index_;
};

}

#endif