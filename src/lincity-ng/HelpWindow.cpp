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

#include <cassert>                  // for assert
#include <exception>                // for exception
#include <functional>               // for bind, _1, function, _2
#include <iostream>                 // for basic_ostream, operator<<, cerr
#include <memory>                   // for unique_ptr
#include <sstream>                  // for basic_ostringstream
#include <stdexcept>                // for runtime_error
#include <utility>                  // for move

#include "Config.hpp"               // for getConfig, Config
#include "Util.hpp"                 // for getButton
#include "gui/Button.hpp"           // for Button
#include "gui/Component.hpp"        // for Component
#include "gui/ComponentLoader.hpp"  // for loadGUIFile
#include "gui/Document.hpp"         // for Document
#include "gui/ScrollView.hpp"       // for ScrollView
#include "gui/Signal.hpp"           // for Signal
#include "gui/Window.hpp"           // for Window
#include "gui/WindowManager.hpp"    // for WindowManager
#include "main.hpp"                 // for getLang
#include "util/ptrutil.hpp"         // for dynamic_unique_cast

using namespace std::placeholders;

HelpWindow::HelpWindow(WindowManager* wm) : windowManager(wm) {
  assert(wm);
}

HelpWindow::~HelpWindow() { }

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
            std::unique_ptr<Window> helpWindowUniq =
              dynamic_unique_cast<Window>(loadGUIFile("gui/helpwindow.xml"));
            helpWindow = helpWindowUniq.get();
            windowManager->addWindow(std::move(helpWindowUniq));
            // connect history back button
            historyBackButton = getButton(*helpWindow, "HistoryBack");
            historyBackButton->clicked.connect(
              std::bind(&HelpWindow::historyBackClicked, this, _1));
        }
        // load new contents
        std::filesystem::path filename = getHelpFile(topic);
        filename = std::filesystem::absolute(filename);
        std::unique_ptr<Document> contents = dynamic_unique_cast<Document>(
          loadGUIFile(filename));
        if(!contents)
          throw std::runtime_error("Help Contents is not a Document");
        contents->linkClicked.connect(
          std::bind(&HelpWindow::linkClicked, this, _1, _2));

        // attach to help window
        Component* helpScrollView
            = helpWindow->findComponent("HelpScrollView");
        if(helpScrollView == 0)
            throw std::runtime_error("HelpScrollView not found in HelpWindow");
        ScrollView* scrollView = dynamic_cast<ScrollView*> (helpScrollView);
        if(scrollView == 0)
            throw std::runtime_error("HelpScrollView is not a ScrollView");
        scrollView->replaceContents(std::move(contents));
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
  std::string language = getLang();
  if(language == "C" || language == "POSIX"
    || language.substr(0,2) == "C."
    || language.substr(0,6) == "POSIX."
  ) {
    getConfig()->appDataDir.get() / "help" / "en" / (topic + ".xml");
  }

  std::filesystem::path filename = getConfig()->appDataDir.get() / "help";
  filename /= language;
  filename /= topic;
  filename += ".xml";
  if(std::filesystem::exists(filename))
    return filename;

  // try stripping the codeset
  std::string::size_type pos = language.find(".");
  if(pos != std::string::npos) {
    language = std::string(language, 0, pos);
    filename = getConfig()->appDataDir.get() / "help";
    filename /= language;
    filename /= topic;
    filename += ".xml";
    if(std::filesystem::exists(filename))
      return filename;
  }

  // try short language, eg. "de" instead of "de_CH"
  pos = language.find("_");
  if(pos != std::string::npos) {
    language = std::string(language, 0, pos);
    filename = getConfig()->appDataDir.get() / "help";
    filename /= language;
    filename /= topic;
    filename += ".xml";
    if(std::filesystem::exists(filename))
      return filename;
  }

  // try english
  filename = getConfig()->appDataDir.get() / "help" / "en";
  filename /= topic;
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
