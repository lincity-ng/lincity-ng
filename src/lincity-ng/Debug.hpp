#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__

#ifndef NDEBUG

#include <string>
#include <sstream>
#include <iostream>

#define cdebug(x) std::cout<<"("<<__FILE__<<":"<<__LINE__<<":"<<__PRETTY_FUNCTION__<<"):"<<x<<std::endl

#define TRACE D test__LINE__(toString(__FILE__)+toString(" ")+toString(__LINE__)+toString(" ")+toString( __PRETTY_FUNCTION__))
#define CTRACE D test__LINE__(toString(__FILE__)+toString(" ")+toString(__LINE__)+toString(" ")+toString( __PRETTY_FUNCTION__)+toString(" ")+toString(((void*)this)))
template<class T>
inline std::string toString(const T&t)
{
  std::ostringstream os;
  os<<t;
  return os.str();
}


class D
{
  std::string m;
  static int d;

 public:
  D(std::string c):
    m(c)
    {
      indent();
      std::cout<<"start of:"<<c<<std::endl;
      d++;
    }
  ~D()
    {
      d--;
      indent();
      std::cout<<"end   of:"<<m<<std::endl;
    }
private:
  void indent()
  {
    for(int i=0;i<d;i++)
      std::cout<<"  ";
  }
};

class LCexception: public std::exception
{
  public:
  LCexception(std::string s):w(s)
  {
  }
  virtual ~LCexception() throw ()
  {
  }
  
  const char*what() const throw ()
  {
    return w.c_str();
  }
  
  private:
  std::string w;
};

template<class T,class Y>
T*checked_cast(Y *x)
{
  if(!x)
    throw LCexception("checked_cast failed:input==0!");
  T*t=dynamic_cast<T*>(x);
  if(!t)  
    throw LCexception("checked_cast failed:cast failed!");
  return t;
}

#endif

#endif
