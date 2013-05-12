
#include "exception.hpp"


Exception::Exception (const char* file, const int line, const char* func, const char* fmt, ...) {
  va_list ap;
  va_list aq;

  va_start(ap, fmt);
  va_copy(aq, ap);
  
  const int size = vsnprintf(NULL, 0, fmt, ap) + 1;
  _foo = new char[size];
  vsnprintf(_foo, size, fmt, aq);

  va_end(ap);
  va_end(aq);
}

const char* Exception::message () const {
  return _foo;
}

