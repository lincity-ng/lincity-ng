#include <config.h>

#include "HelpWindow.hpp"

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <physfs.h>
#include "tinygettext/gettext.hpp"
#include "gui/Component.hpp"
#include "gui/ScrollView.hpp"
#include "gui/Document.hpp"
#include "gui/Paragraph.hpp"
#include "gui/Desktop.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/callback/Callback.hpp"

HelpWindow::HelpWindow(Desktop* desktop)
{
    this->desktop = desktop;
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
        Component* helpWindow = desktop->findComponent("HelpWindow");
        if(helpWindow == 0) {
            helpWindow = loadGUIFile("gui/helpwindow.xml");
            desktop->addChildComponent(helpWindow);
        }
        // load new contents
        std::string filename = getHelpFile(topic);
        std::auto_ptr<Component> contents (loadGUIFile(filename));
        Document* document = dynamic_cast<Document*> (contents.get());
        if(document == 0)
            throw std::runtime_error("Help Contents is not a Document");
        document->linkClicked.connect(
            makeCallback(*this, &HelpWindow::linkClicked));

        // attach to help window
        Component* helpScrollView
            = helpWindow->findComponent("HelpScrollView");
        if(helpScrollView == 0)
            throw std::runtime_error("HelpScrollView not found in HelpWindow");
        ScrollView* scrollView = dynamic_cast<ScrollView*> (helpScrollView);
        if(scrollView == 0)
            throw std::runtime_error("HelpScrollView is not a ScrollView");
        scrollView->replaceContents(contents.release());
    } catch(std::exception& e) {
        std::cerr << "Couldn't open HelpWindow: "
            << e.what() << "\n";
    }
    nextTopic = "";
}

std::string
HelpWindow::getHelpFile(const std::string& topic)
{
    // try in user language
    std::string filename = "help/";
    filename += dictionaryManager->get_language() + "/";
    filename += topic;
    filename += ".xml";
    if(PHYSFS_exists(filename.c_str()))
       return filename;

    filename = "help/en/";
    filename += topic;
    filename += ".xml";
    if(!PHYSFS_exists(filename.c_str())) {
        std::ostringstream msg;
        msg << "There exists no help file for topic '" << topic << "'";
        throw std::runtime_error(msg.str());
    }

    return filename;
}

void
HelpWindow::linkClicked(Paragraph*, const std::string& href)
{
    nextTopic = href;
}
