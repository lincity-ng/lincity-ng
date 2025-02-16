/* ---------------------------------------------------------------------- *
 * src/lincity/exceptions.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2025 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __LINCITYNG_LINCITY_EXCEPTIONS_HPP__
#define __LINCITYNG_LINCITY_EXCEPTIONS_HPP__

#include <exception>
#include <string>

class LincityException : public std::exception {
public:
  LincityException(std::string whatMsg);

  virtual const char *what() const noexcept;

private:
  const std::string whatMsg;
};

class IllegalActionException : public LincityException {
public:
  IllegalActionException(std::string action = "", std::string reason = "");

  const std::string action;
  const std::string reason;
};

class OutOfMoneyException : public IllegalActionException {
public:
  OutOfMoneyException(std::string action = "", bool allowCredit = true);

  const bool usingCredit;
};

#endif // __LINCITYNG_LINCITY_EXCEPTIONS_HPP__
