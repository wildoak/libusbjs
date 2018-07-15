#ifndef libusbjs_EXCEPTION_H_
#define libusbjs_EXCEPTION_H_

#include <napi.h>

namespace libusbjs {

void throw_js(Napi::Env env, const std::string& code, const std::string& message);
bool failed_and_throw_js(Napi::Env env, int errOrNumber);

}
#endif