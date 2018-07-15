#include "util.h"

#include <libusb.h>

namespace libusbjs {

void throw_js(Napi::Env env, const std::string& code, const std::string& message) {
  auto&& error = Napi::Error::New(env, code + ": " + message);
  error.Set("code", Napi::String::New(env, code));
  error.ThrowAsJavaScriptException();
}

bool failed_and_throw_js(Napi::Env env, int errOrNum) {
  // errors are negative
  // some libusb functions return size or error, thus
  // 0, 1, 5 means size
  // -1, -3 means error
  if (errOrNum >= LIBUSB_SUCCESS) {
    return false;
  }

  throw_js(env, libusb_error_name(errOrNum), "libusb call failed");
  return true;
}

}