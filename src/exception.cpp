
#include "exception.hpp"


Exception::Exception (const char* file, const int line, const char* func, const char* fmt, ...) {
  va_list ap;
  va_list aq;

  va_start(ap, fmt);
  va_copy(aq, ap);
  
  const char* prefix_fmt = "%s:%d %s(): ";
  const int prefix_size = snprintf(NULL, 0, prefix_fmt, file, line, func) + 1;

  const int size = prefix_size + vsnprintf(NULL, 0, fmt, ap) + 1;
  _msg = new char[size];
  snprintf(_msg, prefix_size, prefix_fmt, file, line, func);
  vsnprintf(_msg + prefix_size - 1, size, fmt, aq);

  va_end(ap);
  va_end(aq);
}

const char* Exception::message () const {
  return _msg;
}

