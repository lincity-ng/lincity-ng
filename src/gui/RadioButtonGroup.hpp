/* ---------------------------------------------------------------------- *
 * src/gui/RadioButtonGroup.hpp
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

#include "Signal.hpp"

class CheckButton;

// for now, this is hardcoded with CheckButton
class RadioButtonGroup {
public:
  RadioButtonGroup();
  virtual ~RadioButtonGroup();

  Signal<RadioButtonGroup *, CheckButton *> selected;

  void select(CheckButton *button);
  void trySelect(CheckButton *button);
  CheckButton *getSelection() const { return selection; }

  void registerButton(CheckButton *button);

private:
  CheckButton *selection;

  void buttonChecked(CheckButton *button);
  void buttonUnchecked(CheckButton *button);
};
