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
#ifndef __CALLBACK_HPP__
#define __CALLBACK_HPP__

#include "CallbackBase.hpp"

/**
 * \relates make_callback
 * A callback class. Think of it as a C++ function pointer.
 * There's a convenience methos called ::make_callback that simplifies
 * creation of Callback objects.
 *
 * It is a typesafe wrapper around a pointer to member function.
 */
template<typename T, typename Arg1T, typename Arg2T = void>
class Callback : public CallbackBase<Arg1T, Arg2T>
{
public:
    typedef void(T::*Func) (Arg1T, Arg2T);

    Callback(T& _object, Func _func)
        : object(&_object), func(_func)
    { }

    virtual void operator() (Arg1T arg1, Arg2T arg2) const
    {
        (object->*func) (arg1, arg2);
    }

private:
    T* object;
    Func func;
};

template<typename T, typename Arg1T>
class Callback<T, Arg1T> : public CallbackBase<Arg1T>
{
public:
    typedef void(T::*Func) (Arg1T);

    Callback(T& _object, Func _func)
        : object(&_object), func(_func)
    { }

    virtual void operator() (Arg1T arg1) const
    {
        (object->*func) (arg1);
    }

private:
    T* object;
    Func func;
};

/**
 * Create a new callback object.
 *
 * example
 * class A {
 *     void func(int b);
 * };
 * 
 * Signal<int> signal;
 * A a;
 * signal.connect(make_callback(a, &A::func);
 * endexample
 */
template<typename T, typename Arg1T>
Callback<T, Arg1T>* makeCallback(T& object, void(T::*func) (Arg1T))
{
    return new Callback<T, Arg1T> (object, func);
}

template<typename T, typename Arg1T, typename Arg2T>
Callback<T, Arg1T, Arg2T>* makeCallback(T& object, void(T::*func) (Arg1T, Arg2T))
{
    return new Callback<T, Arg1T, Arg2T> (object, func);
}

#endif

/** @file gui/callback/Callback.hpp */

