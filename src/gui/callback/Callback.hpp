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
