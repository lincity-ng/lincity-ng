/* ---------------------------------------------------------------------- *
 * src/gui/RadioButtonGroup.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2024 David Bears <dbear4q@gmail.com>
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

#include "RadioButtonGroup.hpp"

#include <functional>
#include <cstddef>

#include "CheckButton.hpp"

using namespace std::placeholders;

RadioButtonGroup::RadioButtonGroup() :
  selection(NULL)
{}

RadioButtonGroup::~RadioButtonGroup() {}

void RadioButtonGroup::registerButton(CheckButton *btn) {
  btn->checked.connect(
    std::bind(&RadioButtonGroup::buttonChecked, this, _1));
  btn->unchecked.connect(
    std::bind(&RadioButtonGroup::buttonUnchecked, this, _1));

  if(btn->isChecked() && selection != btn) {
    if(selection)
      btn->uncheck();
    else
      buttonChecked(btn);
  }
}

void RadioButtonGroup::buttonChecked(CheckButton *btn) {
  if(selection != btn) {
    auto old = selection;
    selection = btn; // must do this first to avoid firing an event on uncheck
    if(old)
      old->uncheck();
    selected(this, btn);
  }
}
void RadioButtonGroup::buttonUnchecked(CheckButton *btn) {
  if(selection == btn) {
    selection = NULL;
    selected(this, NULL);
  }
}

void RadioButtonGroup::select(CheckButton *button) {
  button->check();
}

void RadioButtonGroup::trySelect(CheckButton *button) {
  button->tryCheck();
}
