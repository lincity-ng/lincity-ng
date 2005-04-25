#ifndef __SIGNAL_HPP__
#define __SIGNAL_HPP__

#include <list>
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

