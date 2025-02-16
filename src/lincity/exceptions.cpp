/* ---------------------------------------------------------------------- *
 * src/lincity/exceptions.cpp
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

#include "exceptions.hpp"

LincityException::LincityException(std::string whatMsg) : whatMsg(whatMsg) {}

const char *
LincityException::what() const noexcept {
  return whatMsg.c_str();
}

IllegalActionException::IllegalActionException(
  std::string action, std::string reason
) :
  LincityException("Cannot " +
    (action != "" ? action : "perform this action") +
    (reason != "" ? " because "+reason : "") +
    "."),
  action(action), reason(reason)
{}

OutOfMoneyException::OutOfMoneyException(
  std::string action, bool usingCredit
) :
  IllegalActionException(action, usingCredit
    ? "you do not have sufficient available credit"
    : "the legislative council will not allow you to do this on credit"),
  usingCredit(usingCredit)
{}
