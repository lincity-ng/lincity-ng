/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>
Copyright (C) 2024 David Bears <dbear4q@gmail.com>

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

#ifndef __SIGNAL_HPP__
#define __SIGNAL_HPP__

#include <assert.h>
#include <functional>
#include <list>

template<typename... Args>
class Signal {
public:
  Signal() {}
  ~Signal() {}

  void operator()(Args... args) const {
    for(auto cb : callbacks) {
      cb(args...);
    }
  }

  void connect(std::function<void(Args...)> callback) {
      callbacks.push_back(callback);
  }

  void clear() {
      callbacks.clear();
  }

private:
    Signal(const Signal& other)
    { assert(false); }
    void operator=(const Signal& other)
    { assert(false); }

    std::list<std::function<void(Args...)>> callbacks;
};

#endif


/** @file gui/Signal.hpp */
