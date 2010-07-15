/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>

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

#include <list>
#include <assert.h>
#include "CallbackBase.hpp"

template<typename Arg1T, typename Arg2T = void>
class Signal
{
public:
    typedef std::list< CallbackBase<Arg1T, Arg2T>* > Callbacks;
    
    Signal()
    { }

    ~Signal()
    {
        for(typename Callbacks::iterator i = callbacks.begin();
                i != callbacks.end(); ++i)
            delete *i;
    }

    void operator() (Arg1T arg1, Arg2T arg2) const
    {
        for(typename Callbacks::const_iterator i = callbacks.begin();
                i != callbacks.end(); ++i) {
            (*(*i)) (arg1, arg2);
        }
    }

    void connect(CallbackBase<Arg1T, Arg2T>* callback)
    {
        callbacks.push_back(callback);
    }

    void clear()
    {
        callbacks.clear();
    }

private:
    Signal(const Signal& other)
    { assert(false); }
    void operator= (const Signal& other)
    { assert(false); }
    
    Callbacks callbacks;
};

template<typename Arg1T>
class Signal<Arg1T>
{
public:
    typedef std::list< CallbackBase<Arg1T>* > Callbacks;
    
    Signal()
    { }

    ~Signal()
    {
        for(typename Callbacks::iterator i = callbacks.begin();
                i != callbacks.end(); ++i)
            delete *i;
    }

    void operator() (Arg1T arg1) const
    {
        for(typename Callbacks::const_iterator i = callbacks.begin();
                i != callbacks.end(); ++i) {
            (*(*i)) (arg1);
        }
    }

    void connect(CallbackBase<Arg1T>* callback)
    {
        callbacks.push_back(callback);
    }

    void clear()
    {
        callbacks.clear();
    }

private:
    Callbacks callbacks;
};

#endif


/** @file gui/callback/Signal.hpp */

