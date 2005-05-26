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
    //thats the value oldgui uses, so saved data has same scale as new.
    int h = 64; //MONTHGRAPH_H; 

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
    
    //sustainability check from do_sust_barchart
    if (sust_dig_ore_coal_count >= SUST_ORE_COAL_YEARS_NEEDED
        && sust_port_count >= SUST_PORT_YEARS_NEEDED
        && sust_old_money_count >= SUST_MONEY_YEARS_NEEDED
        && sust_old_population_count >= SUST_POP_YEARS_NEEDED
        && sust_old_tech_count >= SUST_TECH_YEARS_NEEDED
        && sust_fire_count >= SUST_FIRE_YEARS_NEEDED)
    {
        if (sustain_flag == 0){
	        ok_dial_box ("sustain.mes", GOOD, 0L);
            sustain_flag = 1;
        } else {
            sustain_flag = 0;
        }
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
    setDirty();
}

void EconomyGraph::drawHistoryLineGraph( Painter& painter, Rect2D mg ){ 
    // see oldgui/screen.cpp do_history_linegraph 
    Vector2 a;
    Vector2 b;

    Color red, yellow, blue, brown, grey;
    red.parse( "red");
    yellow.parse( "yellow" );
    blue.parse( "blue" );
    brown.parse( "brown" );
    grey.parse("#A9A9A9FF");

    painter.setClipRectangle( mg ); 
    painter.setFillColor( grey );
    painter.fillRectangle( mg );
    int mgX = (int) mg.p1.x;
    int mgY = (int) mg.p1.y;
    int mgW = (int) mg.getWidth(); 
    int mgH = (int) mg.getHeight();
    
    float scale = (float) mgH / 64; //MONTHGRAPH_H  ;
    
    b.y = mgY + mgH;
    for( int i = mgW - 1; i >= 0; i-- ){
        painter.setLineColor( yellow );
        a.x = mgX + mgW - i;
        a.y = mgY + mgH - scale * monthgraph_nojobs[i];
        
        b.x = mgX + mgW - i;
        painter.drawLine( a, b );
        painter.setLineColor( red );
        a.y = mgY + mgH - scale * monthgraph_starve[i];
        painter.drawLine( a, b );
    }                  
    for( int i = mgW - 1; i > 0; i-- ){
        painter.setLineColor( brown );
        a.x = mgX + mgW - i;
        a.y = mgY + mgH - scale * monthgraph_pop[ i ];
        b.x = mgX + mgW - i-1;
        b.y = mgY + mgH - scale * monthgraph_pop[ i-1 ];
        painter.drawLine( a, b );
        a.y = mgY + mgH - scale * monthgraph_ppool[ i ];
        b.y = mgY + mgH - scale * monthgraph_ppool[ i-1 ];
        painter.setLineColor( blue );
        painter.drawLine( a, b );
    }
    painter.clearClipRectangle();


}

void EconomyGraph::drawSustBarGraph( Painter& painter, Rect2D mg ){ 
    // see oldgui/screen.cpp do_sust_barchart
    Color grey,yellow,orange,black,green,blue,red;
    grey.parse( "#A9A9A9FF" );
    yellow.parse( "yellow" );
    orange.parse( "orange" );
    black.parse( "black" );
    green.parse( "green" );
    blue.parse( "blue" );
    red.parse( "red" );

    painter.setFillColor( grey );
    painter.fillRectangle( mg );
  
    int mgX = (int) mg.p1.x;
    int mgY = (int) mg.p1.y;
    int mgW = (int) mg.getWidth(); 
    int mgH = (int) mg.getHeight();

    Vector2 a, b;
    
    //
#define SUST_BAR_H      5
#define SUST_BAR_GAP_Y  5
 
#if 0 
	/* write the "informative" text */
	Fgl_setfontcolors (0, TEXT_FG_COLOUR);
	Fgl_write (mg->x+3, mg->y + SUST_BAR_GAP_Y - 1,
		   /* TRANSLATORS: 
		      MIN=Mining, PRT=Import/export from port,
		      MNY=Money, POP=Population, TEC=Technology,
		      FIR=Fire coverage
		   */
		   _("MIN"));
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   _("PRT"));
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 2 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   _("MNY"));
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 3 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   _("POP"));
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 4 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   _("TEC"));
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 5 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   _("FIR"));
	Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
#endif
    
	/* draw the starting line */
    a.x = mgX + 38;
    a.y = mgY;
    b.x = mgX + 38;
    b.y = mgY + mgH;
    painter.setLineColor( yellow );
    painter.drawLine( a, b);
    
    Rect2D bar;
    bar.p1.x = mgX + 36;
    bar.p1.y = mgY + SUST_BAR_GAP_Y;
    bar.setHeight( SUST_BAR_H );
    int maxBarLen = mgW - 40;
    int newLen;
    int len; 
    
	/* ore coal */
    newLen = maxBarLen * sust_dig_ore_coal_count / SUST_ORE_COAL_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( orange );
    painter.fillRectangle( bar );
    
	/* import export */
    newLen = maxBarLen * sust_port_count / SUST_PORT_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( black );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    
	/* money */
    newLen = maxBarLen * sust_old_money_count / SUST_MONEY_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( green );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    
	/* population */
    newLen = maxBarLen * sust_old_population_count / SUST_POP_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( blue );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    
	/* tech */
    newLen = maxBarLen * sust_old_tech_count / SUST_TECH_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( yellow );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    
	/* fire */
    newLen = maxBarLen * sust_fire_count / SUST_FIRE_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( red );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    
}


void EconomyGraph::drawFPSGraph( Painter& painter, Rect2D fpsRect ){ 
    Color grey, blue;
    blue.parse( "blue" );
    grey.parse("#A9A9A9FF");
    int mgX = (int) fpsRect.p1.x;
    int mgY = (int) fpsRect.p1.y;
    int mgW = (int) fpsRect.getWidth(); 
    int mgH = (int) fpsRect.getHeight();

    
    painter.setFillColor( grey );
    painter.fillRectangle( fpsRect );
    
    painter.setClipRectangle( fpsRect ); 
      
    Vector2 a;
    Vector2 b;
    painter.setLineColor( blue );
    
    float scale = (float) mgH / 64; //MONTHGRAPH_H  ;

    b.y = mgY + mgH;
    for( int i = mgW - 1; i >= 0; i-- ){
        a.x = mgX + mgW - i;
        a.y = mgY + mgH - scale * fps[i];
        
        b.x = mgX + mgW - i;
        painter.drawLine( a, b );
    }
    painter.clearClipRectangle();
}
    
void EconomyGraph::draw( Painter& painter ){
   
    Color white;
    white.parse( "white" );
    
    Rect2D background( 0, 0, getWidth(), getHeight() );
    int mgX = border;
    int mgY = border;
    int mgW = getConfig()->monthgraphW; 
    int mgH = getConfig()->monthgraphH;
    
    painter.setFillColor( white );
    painter.fillRectangle( background );

    //Draw HistoryLineGraph
    Rect2D currentGraph( mgX, mgY, mgW+border, mgH+border );
    drawHistoryLineGraph( painter, currentGraph );

    //Draw Sustainability Bars
    currentGraph.move( Vector2( 0, border + mgH ) );
    drawSustBarGraph( painter, currentGraph);

    //Draw FPS-Window
    currentGraph.move( Vector2( 0, border + mgH ) );
    currentGraph.setHeight( mgH/2 );
    drawFPSGraph( painter, currentGraph );
}

IMPLEMENT_COMPONENT_FACTORY(EconomyGraph);
