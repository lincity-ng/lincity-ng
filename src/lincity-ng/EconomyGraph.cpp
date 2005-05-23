/**
 *  EconomyGraph Component for Lincity-NG.
 *   
 *  May 2005, Wolfgang Becker <uafr@gmx.de>
 *
 */
#include <config.h>

#include "EconomyGraph.hpp"

#include "gui/ComponentFactory.hpp"
#include <iostream>


EconomyGraph::EconomyGraph(){

}

EconomyGraph::~EconomyGraph(){

}

void EconomyGraph::parse( XmlReader& reader ){
    XmlReader::AttributeIterator iter( reader );
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(name, value)){
            continue;
        } else {
            std::cerr << "Unknown attribute '" << name 
                      << "' skipped in PBar.\n";
        }
    }
}
 
void EconomyGraph::draw( Painter& painter ){
    //TODO: see oldgui/screen.cpp
    //do_history_linegraph looks like a starting point.
}

IMPLEMENT_COMPONENT_FACTORY(EconomyGraph);
