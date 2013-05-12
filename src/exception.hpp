#ifndef EXCEPTION_HPP_GGSANEU2
#define EXCEPTION_HPP_GGSANEU2

#include <stdexcept>
#include <cstdio>
#include <cstdarg>


class Exception : public std::exception {
  public:
    Exception (const char* file, const int line, const char* func, const char* fmt, ...);
    const char* message () const;

  private:
    char* _msg;
};


#define raise(fmt,...) throw Exception(__FILE__, __LINE__, __func__, fmt, __VA_ARGS__)


#endif /* end of include guard: EXCEPTION_HPP_GGSANEU2 */

