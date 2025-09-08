/* ---------------------------------------------------------------------- *
 * src/gui/DialogBuilder.cpp
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

#include "DialogBuilder.hpp"

#include <filesystem>           // for path
#include <map>                  // for map
#include <memory>               // for unique_ptr
#include <stdexcept>            // for runtime_error
#include <utility>              // for move

#include "Button.hpp"           // for Button
#include "Child.hpp"            // for Child
#include "Component.hpp"        // for Component
#include "ComponentLoader.hpp"  // for loadGUIFile
#include "Document.hpp"         // for Document
#include "Image.hpp"            // for Image
#include "Paragraph.hpp"        // for Paragraph
#include "Signal.hpp"           // for Signal
#include "Style.hpp"            // for Style, styleRegistry
#include "SwitchComponent.hpp"  // for SwitchComponent
#include "Window.hpp"           // for Window
#include "WindowManager.hpp"    // for WindowManager
#include "util/ptrutil.hpp"     // for dynamic_unique_cast

WindowManager *DialogBuilder::defaultWm = nullptr;

DialogBuilder::DialogBuilder() : _windowManager(nullptr) {
  _message.reset(new Document());
}

DialogBuilder::~DialogBuilder() {
}

DialogBuilder&
DialogBuilder::titleText(const std::string& title) {
  _titleText = title;

  return *this;
}

DialogBuilder&
DialogBuilder::messageAddText(const std::string& content) {
  std::unique_ptr<Paragraph> par(new Paragraph());
  Style& messageStyle = styleRegistry.at("message");
  par->setText(content, messageStyle);
  _message->addParagraph(std::move(par));

  return *this;
}

DialogBuilder&
DialogBuilder::messageAddTextBold(const std::string& content) {
  std::unique_ptr<Paragraph> par(new Paragraph());
  Style& messageBoldStyle = styleRegistry.at("messagebold");
  par->setText(content, messageBoldStyle);
  _message->addParagraph(std::move(par));

  return *this;
}

DialogBuilder&
DialogBuilder::image(std::unique_ptr<Image>&& image) {
  _image = std::move(image);

  return *this;
}

DialogBuilder&
DialogBuilder::imageFile(const std::string& image) {
  if(!_image) {
    _image.reset(new Image());
  }
  _image->setFile(image);

  return *this;
}

DialogBuilder&
DialogBuilder::buttonSet(ButtonSet buttonSet) {
  _buttonSet = buttonSet;

  return *this;
}

DialogBuilder&
DialogBuilder::windowManager(WindowManager *windowManager) {
  _windowManager = windowManager;

  return *this;
}

DialogBuilder&
DialogBuilder::onOk(std::function<void()> callback) {
  okCallbacks.push_back(callback);

  return *this;
}

DialogBuilder&
DialogBuilder::onYes(std::function<void()> callback) {
  yesCallbacks.push_back(callback);

  return *this;
}

DialogBuilder&
DialogBuilder::onNo(std::function<void()> callback) {
  noCallbacks.push_back(callback);

  return *this;
}

void
DialogBuilder::build() {
  std::unique_ptr<Window> dialog = dynamic_unique_cast<Window>(
    loadGUIFile("gui/dialog.xml"));
  Paragraph *title = dynamic_cast<Paragraph *>(dialog->findComponent("title"));
  Child *messageChild = dialog->findComponent("message")->getParentChild();
  Child *imageChild = dialog->findComponent("image")->getParentChild();
  SwitchComponent *buttonSwitch = dynamic_cast<SwitchComponent *>(
    dialog->findComponent("buttons"));

  title->setText(_titleText);

  messageChild->setComponent(std::move(_message));

  if(_image) {
    imageChild->setComponent(std::move(_image));
  }

  if(!_windowManager)
    _windowManager = defaultWm;
  if(!_windowManager)
    throw std::runtime_error("no window manager to display dialog");

  std::function<void(Button *)> closeAction =
    std::bind(&WindowManager::removeWindow, _windowManager, dialog.get());

  switch(_buttonSet) {
  case DialogBuilder::ButtonSet::OK: {
    buttonSwitch->switchComponent("ok-buttonset");

    Button *okButton = dynamic_cast<Button *>(
      buttonSwitch->getActiveComponent()->findComponent("ok-button"));
    okButton->clicked.connect(closeAction);
    for(std::function<void()>& okcb : okCallbacks) {
      okButton->clicked.connect(std::bind(okcb));
    }
  }
    break;

  case DialogBuilder::ButtonSet::YESNO: {
    buttonSwitch->switchComponent("yesno-buttonset");

    Button *yesButton = dynamic_cast<Button *>(
      buttonSwitch->getActiveComponent()->findComponent("yes-button"));
    Button *noButton = dynamic_cast<Button *>(
      buttonSwitch->getActiveComponent()->findComponent("no-button"));
    yesButton->clicked.connect(closeAction);
    noButton->clicked.connect(closeAction);
    for(std::function<void()>& yescb : yesCallbacks) {
      yesButton->clicked.connect(std::bind(yescb));
    }
    for(std::function<void()>& nocb : noCallbacks) {
      noButton->clicked.connect(std::bind(nocb));
    }
  }
    break;
  }

  dialog->resize(dialog->getWidth(), dialog->getHeight()); // force relayout
  _windowManager->addWindow(std::move(dialog));
}

void
DialogBuilder::setDefaultWindowManager(WindowManager *defaultWm) {
  DialogBuilder::defaultWm = defaultWm;
}
