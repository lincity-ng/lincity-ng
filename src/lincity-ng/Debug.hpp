/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
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

/** @file lincity-ng/Debug.hpp */

