/* ---------------------------------------------------------------------- *
 * src/gui/DialogBuilder.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
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

#include <functional>  // for function
#include <list>        // for list
#include <memory>
#include <string>      // for string, basic_string

class Document;
class Image;
class WindowManager;

class DialogBuilder {
public:
  enum class ButtonSet {
    OK,
    YESNO
  };

  DialogBuilder();
  ~DialogBuilder();

  DialogBuilder& titleText(const std::string& title);
  // DialogBuilder& message(Document *content);
  DialogBuilder& messageAddText(const std::string& content);
  DialogBuilder& messageAddTextBold(const std::string& content);
  DialogBuilder& image(std::unique_ptr<Image>&& image);
  DialogBuilder& imageFile(const std::string& image);
  DialogBuilder& buttonSet(ButtonSet buttonSet);
  DialogBuilder& windowManager(WindowManager *windowManager);

  DialogBuilder& onOk(std::function<void()> callback);
  DialogBuilder& onYes(std::function<void()> callback);
  DialogBuilder& onNo(std::function<void()> callback);

  void build();

  static void setDefaultWindowManager(WindowManager *defaultWm);

private:
  std::string _titleText;
  std::unique_ptr<Document> _message;
  std::unique_ptr<Image> _image;
  ButtonSet _buttonSet;
  // std::filesystem::path template;
  WindowManager *_windowManager;
  std::list<std::function<void()>> okCallbacks;
  std::list<std::function<void()>> yesCallbacks;
  std::list<std::function<void()>> noCallbacks;

  static WindowManager *defaultWm;
};
