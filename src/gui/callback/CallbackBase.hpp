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


/** @file gui/callback/CallbackBase.hpp */

