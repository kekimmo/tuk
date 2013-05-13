
#include <cstdarg>
#include "string.hpp"


std::string format (const char* fmt, ...) {
  va_list ap;
  va_list aq;
  va_start(ap, fmt);
  va_copy(aq, ap);
  
  const size_t size = vsnprintf(NULL, 0, fmt, ap) + 1;
  char* buffer = new char[size];
  vsnprintf(buffer, size, fmt, aq);

  std::string s(buffer);
  delete buffer;

  va_end(ap);
  va_end(aq);

  return s;
}

