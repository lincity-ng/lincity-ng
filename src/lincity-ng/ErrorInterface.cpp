#include <config.h>

#include <iostream>

void do_error (const char *s)
{
  std::cerr<<s<<std::endl;
}

void HandleError (const char *s, int i)
{
  std::cerr<<"ERROR of degree "<<i<<":"<<s<<std::endl;
}
