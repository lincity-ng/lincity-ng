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
#ifndef __HELPWINDOW_HPP__
#define __HELPWINDOW_HPP__

#include <string>
#include <stack>
#include "gui/Button.hpp"

class Desktop;
class Paragraph;

class HelpWindow
{
public:
    HelpWindow(Desktop* desktop);
    ~HelpWindow();

    void showTopic(const std::string& topic);
    void update();

private:
    void linkClicked(Paragraph* paragraph, const std::string& href);
    void historyBackClicked(Button*);
    std::string getHelpFile(const std::string& topic);

    Desktop* desktop;
    std::string nextTopic;
    std::stack<std::string> topicHistory;
    Button* historyBackButton;
};

#endif

/** @file lincity-ng/HelpWindow.hpp */

