#include <config.h>

#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"
#include "XmlReader.hpp"
#include "Desktop.hpp"

#include <sstream>
#include <stdexcept>
#include <iostream>

ComponentFactories* component_factories = 0;

// import factory
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
    return ::createComponent((const char*) nreader.getName(), nreader);
}

//---------------------------------------------------------------------------

/** It seems ar or g++ strip out unused functions in static libraries. For some
 * reasons also global constructor seem to be considered unused if noone uses
 * the global object. So can't use our slick component factory registration
 * tricks :-/ And have to fill in the list manually here
 */

#include "Button.hpp"
#include "Desktop.hpp"
#include "Document.hpp"
#include "Image.hpp"
#include "Panel.hpp"
#include "Paragraph.hpp"
#include "ScrollBar.hpp"
#include "ScrollView.hpp"
#include "TableLayout.hpp"
#include "Window.hpp"

DECLARE_COMPONENT_FACTORY(Button)
DECLARE_COMPONENT_FACTORY(Desktop)
DECLARE_COMPONENT_FACTORY(Document)
DECLARE_COMPONENT_FACTORY(Image)
DECLARE_COMPONENT_FACTORY(Panel)
DECLARE_COMPONENT_FACTORY(Paragraph)
DECLARE_COMPONENT_FACTORY(ScrollBar)
DECLARE_COMPONENT_FACTORY(ScrollView)
DECLARE_COMPONENT_FACTORY(TableLayout)
DECLARE_COMPONENT_FACTORY(Window)

void initFactories()
{
    static bool initialized = false;
    if(!initialized) {
        new INTERN_ButtonFactory();
        new INTERN_DesktopFactory();
        new INTERN_DocumentFactory();
        new INTERN_ImageFactory();
        new INTERN_PanelFactory();
        new INTERN_ParagraphFactory();
        new INTERN_ScrollBarFactory();
        new INTERN_ScrollViewFactory();
        new INTERN_TableLayoutFactory();
        new INTERN_WindowFactory();
        new ImportFactory();
        initialized = true;
    }
}

