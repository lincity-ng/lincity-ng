#include <iostream>

void do_error (char *s)
{
  std::cerr<<s<<std::endl;
}

void HandleError (char *s, int i)
{
  std::cerr<<"ERROR of degree "<<i<<":"<<s<<std::endl;
}
