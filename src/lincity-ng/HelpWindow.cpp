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

#include "HelpWindow.hpp"

#include <exception>                    // for exception
#include <functional>                   // for bind, function, _1, _2
#include <iostream>                     // for basic_ostream, operator<<, cerr
#include <memory>                       // for unique_ptr
#include <sstream>                      // for basic_ostringstream
#include <stdexcept>                    // for runtime_error

#include "Util.hpp"                     // for getButton
#include "gui/Button.hpp"               // for Button
#include "gui/Component.hpp"            // for Component
#include "gui/ComponentLoader.hpp"      // for loadGUIFile
#include "gui/Desktop.hpp"              // for Desktop
#include "gui/Document.hpp"             // for Document
#include "gui/ScrollView.hpp"           // for ScrollView
#include "gui/Signal.hpp"               // for Signal
#include "gui/Window.hpp"               // for Window
#include "gui/WindowManager.hpp"        // for WindowManager
#include "tinygettext/gettext.hpp"      // for dictionaryManager
#include "tinygettext/tinygettext.hpp"  // for DictionaryManager
#include "Config.hpp"

using namespace std::placeholders;

HelpWindow::HelpWindow(Desktop* desktop)
{
    this->windowManager = dynamic_cast<WindowManager *>(
      desktop->findComponent("windowManager"));
}

HelpWindow::~HelpWindow()
{}

void
HelpWindow::update()
{
    if(nextTopic != "") {
        showTopic(nextTopic);
    }
}

void
HelpWindow::showTopic(const std::string& topic)
{
    try {
        // make sure HelpWindow is open
        Window* helpWindow = dynamic_cast<Window *>(
          windowManager->findComponent("HelpWindow"));
        if(helpWindow == 0) {
            helpWindow = dynamic_cast<Window *>(
              loadGUIFile("gui/helpwindow.xml"));
            windowManager->addWindow(helpWindow);
            // connect history back button
            historyBackButton = getButton(*helpWindow, "HistoryBack");
            historyBackButton->clicked.connect(
              std::bind(&HelpWindow::historyBackClicked, this, _1));
        }
        // load new contents
        std::filesystem::path filename = getHelpFile(topic);
        filename = std::filesystem::absolute(filename);
        std::unique_ptr<Component> contents (loadGUIFile(filename));
        Document* document = dynamic_cast<Document*> (contents.get());
        if(document == 0)
            throw std::runtime_error("Help Contents is not a Document");
        document->linkClicked.connect(
          std::bind(&HelpWindow::linkClicked, this, _1, _2));

        // attach to help window
        Component* helpScrollView
            = helpWindow->findComponent("HelpScrollView");
        if(helpScrollView == 0)
            throw std::runtime_error("HelpScrollView not found in HelpWindow");
        ScrollView* scrollView = dynamic_cast<ScrollView*> (helpScrollView);
        if(scrollView == 0)
            throw std::runtime_error("HelpScrollView is not a ScrollView");
        scrollView->replaceContents(contents.release());
        topicHistory.push(topic);
    } catch(std::exception& e) {
        std::cerr << "Couldn't open HelpWindow: "
            << e.what() << "\n";
    }
    nextTopic = "";
}

std::filesystem::path
HelpWindow::getHelpFile(const std::string& topic)
{
  // try in user language
  std::filesystem::path filename = getConfig()->appDataDir / "help";
  filename /= dictionaryManager->get_language();
  filename /= topic;
  filename += ".xml";
  if(std::filesystem::exists(filename))
    return filename;

  // try short language, eg. "de" instead of "de_CH"
  std::string language = dictionaryManager->get_language();
  std::string::size_type pos = language.find("_");
  if(pos != std::string::npos) {
    language = std::string(language, 0, pos);
    filename = getConfig()->appDataDir / "help";
    filename /= language;
    filename /= topic;
    filename += ".xml";
    if(std::filesystem::exists(filename))
      return filename;
  }

  // try english
  filename = getConfig()->appDataDir / "help" / "en";
  filename += topic;
  filename += ".xml";
  if(std::filesystem::exists(filename))
    return filename;

  // give up
  std::ostringstream msg;
  msg << "There exists no help file for topic '" << topic << "'";
  throw std::runtime_error(msg.str());
}

void
HelpWindow::linkClicked(Paragraph*, const std::string& href)
{
    nextTopic = href;
}

void
HelpWindow::historyBackClicked(Button*)
{
    if(topicHistory.size() > 1) {
        topicHistory.pop(); //the current page is on the top, remove it.
        nextTopic = topicHistory.top();
        topicHistory.pop();
    }
}

/** @file lincity-ng/HelpWindow.cpp */
