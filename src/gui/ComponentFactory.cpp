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

/**
 * @author Matthias Braun
 * @file ComponentFactory.cpp
 */

#include <config.h>

#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"
#include "XmlReader.hpp"
#include "Desktop.hpp"
#include "tinygettext/tinygettext.hpp"
//#include "../lincity-ng/CheckButton.hpp" //FIXME will that help?

#include <sstream>
#include <stdexcept>
#include <iostream>

tinygettext::DictionaryManager* dictionaryGUIManager = 0;

const char * 
GUI_TRANSLATE(const char * msgid)
{
    return dictionaryGUIManager->get_dictionary().translate(msgid);
}

std::string  
GUI_TRANSLATE(const std::string& msgid)
{
    return dictionaryGUIManager->get_dictionary().translate(msgid);
}

ComponentFactories* component_factories = 0;

// import factory
/**
 * @class ImportFactory
 */
class ImportFactory : public Factory
{
public:
    ImportFactory()
    {
        if(component_factories == 0)
            component_factories = new ComponentFactories;

        component_factories->insert(std::make_pair("Import", this));
    }
    
    Component* createComponent(XmlReader& reader);
};
//static ImportFactory factory_Import;

Component*
ImportFactory::createComponent(XmlReader& reader)
{
    std::string importfile;
    
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(strcmp(attribute, "src") == 0) {
            importfile = value;
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }

    if(importfile == "")
        throw std::runtime_error("No src attribute specified.");  
    XmlReader nreader(importfile);
    //std::cout << "importing Factory: " << importfile << std::endl;
    return ::createComponent((const char*) nreader.getName(), nreader);
}

//---------------------------------------------------------------------------

/**
 * @note It seems ar or g++ strip out unused functions in static libraries. For
 * some reasons also global constructor seems to be considered unused if noone
 * uses the global object. So can't use our slick component factory registration
 * tricks :-/ And have to fill in the list manually here
 */

#include "Button.hpp"
#include "Desktop.hpp"
#include "Document.hpp"
#include "FilledRectangle.hpp"
#include "Gradient.hpp"
#include "Image.hpp"
#include "Panel.hpp"
#include "Paragraph.hpp"
#include "ScrollBar.hpp"
#include "ScrollView.hpp"
#include "SwitchComponent.hpp"
#include "TableLayout.hpp"
#include "TooltipManager.hpp"
#include "Window.hpp"

//DECLARE_COMPONENT_FACTORY(CheckButton); //FIXME will this help?
DECLARE_COMPONENT_FACTORY(Button)
DECLARE_COMPONENT_FACTORY(Desktop)
DECLARE_COMPONENT_FACTORY(Document)
DECLARE_COMPONENT_FACTORY(FilledRectangle)
DECLARE_COMPONENT_FACTORY(Gradient)
DECLARE_COMPONENT_FACTORY(Image)
DECLARE_COMPONENT_FACTORY(Panel)
DECLARE_COMPONENT_FACTORY(Paragraph)
DECLARE_COMPONENT_FACTORY(ScrollBar)
DECLARE_COMPONENT_FACTORY(ScrollView)
DECLARE_COMPONENT_FACTORY(SwitchComponent)
DECLARE_COMPONENT_FACTORY(TableLayout)
DECLARE_COMPONENT_FACTORY(TooltipManager)
DECLARE_COMPONENT_FACTORY(Window)

void initFactories()
{
    static bool initialized = false;
    if(!initialized) {
		//new INTERN_CheckButtonFactory(); //FIXME will this help?
        new INTERN_ButtonFactory();
        new INTERN_DesktopFactory();
        new INTERN_DocumentFactory();
        new INTERN_FilledRectangleFactory();
        new INTERN_GradientFactory();
        new INTERN_ImageFactory();
        new INTERN_PanelFactory();
        new INTERN_ParagraphFactory();
        new INTERN_ScrollBarFactory();
        new INTERN_ScrollViewFactory();
        new INTERN_SwitchComponentFactory();
        new INTERN_TableLayoutFactory();
        new INTERN_TooltipManagerFactory();
        new INTERN_WindowFactory();
        new ImportFactory();

        dictionaryGUIManager = new tinygettext::DictionaryManager();
        dictionaryGUIManager->set_charset("UTF-8");
        dictionaryGUIManager->add_directory("locale/gui");

        initialized = true;
    }
}


/** @file gui/ComponentFactory.cpp */

