/**
 *  EconomyGraph Component for Lincity-NG.
 *   
 *  May 2005, Wolfgang Becker <uafr@gmx.de>
 *
 */
#include <config.h>
#include <iostream>

#include "EconomyGraph.hpp"

#include "gui/ComponentFactory.hpp"
#include "gui/Painter.hpp"
#include "gui/Rect2D.hpp"

#include "gui_interface/shared_globals.h"

#include "lincity/shrglobs.h"
#include "lincity/engglobs.h"
#include "lincity/stats.h"
#include "lincity/lin-city.h"

#include "Config.hpp"


EconomyGraph* economyGraphPtr = 0;

EconomyGraph* getEconomyGraph()
{
    return economyGraphPtr;
}

EconomyGraph::EconomyGraph(){
    economyGraphPtr = this;
    fps = (int*) malloc (sizeof(int) * getConfig()->monthgraphW );
    for ( int i = 0; i < getConfig()->monthgraphW; i++) {
        fps[i] = 0;
    }
}

EconomyGraph::~EconomyGraph(){
    if( economyGraphPtr == this ){
        economyGraphPtr = 0;
    }
    free( fps );
}

void EconomyGraph::parse( XmlReader& reader ){
    XmlReader::AttributeIterator iter( reader );
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(name, value)){
            continue;
        } else if(strcmp(name, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse width attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse height attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else {
            std::cerr << "Unknown attribute '" << name 
                      << "' skipped in EconomyGraph.\n";
        }
    }
}

//see do_history_linegraph in oldgui/screen.cpp
void EconomyGraph::updateData(){
    int i;
    float f;
    int w = getConfig()->monthgraphW;
    int h = getConfig()->monthgraphH;

    for (i = w - 1; i > 0; i--) {
	    monthgraph_pop[i] = monthgraph_pop[i-1];
	    monthgraph_ppool[i] = monthgraph_ppool[i-1];
	    monthgraph_nojobs[i] = monthgraph_nojobs[i-1];
	    monthgraph_starve[i] = monthgraph_starve[i-1];
    }
    if (tpopulation > 0)
    {
	monthgraph_pop[0] = ((int) (log ((tpopulation / NUMOF_DAYS_IN_MONTH)
					 + 1) * h / 15)) - 5;
	if (monthgraph_pop[0] < 0)
	    monthgraph_pop[0] = 0;
	f = ((float) tstarving_population 
	     / ((float) tpopulation + 1.0)) * 100.0;
	if (tpopulation > 3000)	/* double the scale if pop > 3000 */
	    f += f;
	if (tpopulation > 6000)	/* double it AGAIN if pop > 6000 */
	    f += f;
	monthgraph_starve[0] = (int) f;
	/* max out at 32 */
	if (monthgraph_starve[0] >= h)
	    monthgraph_starve[0] = h - 1;
	f = ((float) tunemployed_population
	     / ((float) tpopulation + 1.0)) * 100.0;
	if (tpopulation > 3000)	/* double the scale if pop > 3000 */
	    f += f;
	if (tpopulation > 6000)	/* double it AGAIN if pop > 6000 */
	    f += f;
	monthgraph_nojobs[0] = (int) f;
	/* max out at 32  */
	if (monthgraph_nojobs[0] >= h)
	    monthgraph_nojobs[0] = h - 1;
	monthgraph_ppool[0] = ((int) (sqrt (people_pool + 1) * h) / 35);
	if (monthgraph_ppool[0] < 0)
	    monthgraph_ppool[0] = 0;
	if (monthgraph_ppool[0] >= h)
	    monthgraph_ppool[0] = h - 1;
    }
    //redraw
    setDirty();
}

void EconomyGraph::newFPS( int frame ){
    int w = getConfig()->monthgraphW;
    int h = getConfig()->monthgraphH;

    for( int i = w - 1; i > 0; i--) {
        fps[ i ] = fps[i-1];
    }
    fps[ 0 ] = h * frame / 100;
}

void EconomyGraph::draw( Painter& painter ){
    
    Color white, black, red, yellow, blue, brown, grey;
    white.parse( "white" );
    black.parse( "black" );
    red.parse( "red");
    yellow.parse( "yellow" );
    blue.parse( "blue" );
    brown.parse( "brown" );
    grey.parse("#A9A9A9FF");

    Rect2D background( 0, 0, getWidth(), getHeight() );
    int mgX = border;
    int mgY = border;
    int mgW = getConfig()->monthgraphW; 
    int mgH = getConfig()->monthgraphH;
    Rect2D mg( mgX, mgY, mgW+border, mgH+border );
    
    painter.setFillColor( white );
    painter.fillRectangle( background );

    painter.setFillColor( grey );
    painter.fillRectangle( mg );

    // see oldgui/screen.cpp do_history_linegraph 
    Vector2 a;
    Vector2 b;

    painter.setClipRectangle( mg ); 
    b.y = mgY + mgH;
    for( int i = mgW - 1; i >= 0; i-- ){
        painter.setLineColor( yellow );
        a.x = mgX + mgW - i;
        a.y = mgY + mgH - monthgraph_nojobs[i];
        
        b.x = mgX + mgW - i;
        painter.drawLine( a, b );
        painter.setLineColor( red );
        a.y = mgY + mgH - monthgraph_starve[i];
        painter.drawLine( a, b );
    }                  
    for( int i = mgW - 1; i > 0; i-- ){
        painter.setLineColor( brown );
        a.x = mgX + mgW - i;
        a.y = mgY + mgH - monthgraph_pop[ i ];
        b.x = mgX + mgW - i-1;
        b.y = mgY + mgH - monthgraph_pop[ i-1 ];
        painter.drawLine( a, b );
        a.y = mgY + mgH - monthgraph_ppool[ i ];
        b.y = mgY + mgH - monthgraph_ppool[ i-1 ];
        painter.setLineColor( blue );
        painter.drawLine( a, b );
    }
    painter.clearClipRectangle();

    Rect2D fpsRect = mg;
    fpsRect.move( Vector2( 0, border + mgH ) );
    painter.setFillColor( grey );
    painter.fillRectangle( fpsRect );
    painter.setClipRectangle( fpsRect ); 
      
    b.y = 2*(mgY + mgH) ;
    for( int i = mgW - 1; i >= 0; i-- ){
        painter.setLineColor( blue );
        a.x = mgX + mgW - i;
        a.y = 2*(mgY + mgH) - fps[i];
        
        b.x = mgX + mgW - i;
        painter.drawLine( a, b );
    }
    painter.clearClipRectangle();
}

IMPLEMENT_COMPONENT_FACTORY(EconomyGraph);
