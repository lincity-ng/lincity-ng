#include <config.h>

#include "ComponentLoader.hpp"
#include "ComponentFactory.hpp"
#include "XmlReader.hpp"
#include "Desktop.hpp"
#include "Style.hpp"

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <memory>

void initFactories();

Component* createComponent(const std::string& type, XmlReader& reader)
{
    initFactories();
    
    if(component_factories == 0)
        throw std::runtime_error("No component factories registered");
    
    ComponentFactories::iterator i = component_factories->find(type);
    if(i == component_factories->end()) {
        std::stringstream msg;
        msg << "Couldn't find a component factory for '" << type << "'";
        throw std::runtime_error(msg.str());
    }
   
    try {
        return i->second->createComponent(reader);
    } catch(std::exception& e) {
        std::stringstream msg;
        msg << "Error while parsing component '" << type << "': " << e.what();
        throw std::runtime_error(msg.str());
    } catch(...) {
        throw;
    }
}

Component* loadGUIFile(const std::string& filename)
{
    XmlReader reader(filename);
    
    if(strcmp((const char*) reader.getName(), "gui") != 0) {
        std::stringstream msg;
        msg << "XML File '" << filename << "' is not a gui file.";
        throw std::runtime_error(msg.str());
    }

    std::auto_ptr<Desktop> desktop (new Desktop());
    desktop->parse(reader);
    return desktop.release();
}

Component* parseEmbeddedComponent(XmlReader& reader)
{
    Component* component = 0;
    try {
        int depth = reader.getDepth();
        while(reader.read() && reader.getDepth() > depth) {
            if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
                const char* name = (const char*) reader.getName();
                if(strcmp(name, "DefineStyle") == 0) {
                    parseStyleDef(reader);
                } else if(component == 0) {
                    component = createComponent(name, reader);
                } else {
                    std::cerr << "Multiple components specified."
                        << " Skipping '" << name << "'.\n";
                    continue;
                }
            }
        }
    } catch(...) {
        delete component;
        throw;
    }

    return component;
}
