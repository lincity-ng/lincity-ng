#ifndef __CALLBACKBASE_HPP__
#define __CALLBACKBASE_HPP__

template<typename Arg1T, typename Arg2T = void>
class CallbackBase
{
public:
    virtual ~CallbackBase()
    { }
    
    virtual void operator()(Arg1T arg1, Arg2T arg2) const = 0;
};

/** Base class for callbacks */
template<typename Arg1T>
class CallbackBase<Arg1T>
{
public:
    virtual ~CallbackBase()
    { }
    
    virtual void operator()(Arg1T arg) const = 0;
};

#endif

