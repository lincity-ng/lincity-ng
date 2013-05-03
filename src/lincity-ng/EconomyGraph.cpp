/*
Copyright (C) 2005 Wolfgang Becker <uafr@gmx.de>

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
#include <config.h>
#include <iostream>

#include "EconomyGraph.hpp"

#include "gui/ComponentFactory.hpp"
#include "gui/Painter.hpp"
#include "gui/Rect2D.hpp"
#include "gui/Style.hpp"
#include "gui/FontManager.hpp"
#include "gui/TextureManager.hpp"
#include "gui/SwitchComponent.hpp"

#include "gui_interface/shared_globals.h"

#include "tinygettext/gettext.hpp"

#include "lincity/engglobs.h"
#include "lincity/stats.h"
#include "lincity/lin-city.h"
#include "lincity/sustainable.h"

#include "ScreenInterface.hpp"
#include "Config.hpp"
#include "Dialog.hpp"
#include "Util.hpp"

extern void ok_dial_box(const char *, int, const char *);

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
    labelTextureMIN = 0;
    labelTexturePRT = 0;
    labelTextureMNY = 0;
    labelTexturePOP = 0;
    labelTextureTEC = 0;
    labelTextureFIR = 0;
    labelTextureEconomy = 0;
    labelTextureSustainability = 0;
    labelTextureFPS = 0;

    nobodyHomeDialogShown = false;
    switchEconomyGraphButton = NULL;
}

EconomyGraph::~EconomyGraph(){
    if( economyGraphPtr == this ){
        economyGraphPtr = 0;
    }
    free( fps );
    delete labelTextureMIN;
    delete labelTexturePRT;
    delete labelTextureMNY;
    delete labelTexturePOP;
    delete labelTextureTEC;
    delete labelTextureFIR;
    delete labelTextureEconomy;
    delete labelTextureSustainability;
    delete labelTextureFPS;
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
    //Generate Labels for Sustainability Graph
    Style labelStyle;
    labelStyle.font_family = "sans";
    labelStyle.font_size = 10;
    TTF_Font* font = fontManager->getFont( labelStyle );
    SDL_Surface* labelXXX;
		   /* MIN=Mining, PRT=Import/export from port,
		      MNY=Money, POP=Population, TEC=Technology,
		      FIR=Fire coverage
		   */
    labelXXX = TTF_RenderUTF8_Blended( font, _("Mining"), labelStyle.text_color.getSDLColor() );
    labelTextureMIN = texture_manager->create( labelXXX );
    labelXXX = TTF_RenderUTF8_Blended( font, _("Trade"), labelStyle.text_color.getSDLColor() );
    labelTexturePRT = texture_manager->create( labelXXX );
    labelXXX = TTF_RenderUTF8_Blended( font, _("Money"), labelStyle.text_color.getSDLColor() );
    labelTextureMNY = texture_manager->create( labelXXX );
    labelXXX = TTF_RenderUTF8_Blended( font, _("Popul."), labelStyle.text_color.getSDLColor() );
    labelTexturePOP = texture_manager->create( labelXXX );
    labelXXX = TTF_RenderUTF8_Blended( font, _("Techn."), labelStyle.text_color.getSDLColor() );
    labelTextureTEC = texture_manager->create( labelXXX );
    labelXXX = TTF_RenderUTF8_Blended( font, _("Fire"), labelStyle.text_color.getSDLColor() );
    labelTextureFIR = texture_manager->create( labelXXX );

    labelXXX = TTF_RenderUTF8_Blended( font, _("Economy Overview:"), labelStyle.text_color.getSDLColor() );
    labelTextureEconomy = texture_manager->create( labelXXX );

    labelXXX = TTF_RenderUTF8_Blended( font, _("Sustainability:"), labelStyle.text_color.getSDLColor() );
    labelTextureSustainability = texture_manager->create( labelXXX );

    labelXXX = TTF_RenderUTF8_Blended( font, _("Frames per Second:"), labelStyle.text_color.getSDLColor() );
    labelTextureFPS = texture_manager->create( labelXXX );
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
    if (tpopulation > 0) {
        /* log scale 0 -> 200 000 = 10^5.3 */
        float scale = h / (5.3 * log(10.));
        float t = (total_time %NUMOF_DAYS_IN_MONTH + 1.f);

        monthgraph_pop[0] = (int) (log (tpopulation / t) * scale);
        if (monthgraph_pop[0] < 0)
            monthgraph_pop[0] = 0;
        if (monthgraph_pop[0] >= h)
            monthgraph_pop[0] = h - 1;

        monthgraph_starve[0] = (int) (log( tstarving_population  /  t) * scale);
        if (monthgraph_starve[0] < 0)
            monthgraph_starve[0] = 0;
        if (monthgraph_starve[0] >= h)
            monthgraph_starve[0] = h - 1;

        monthgraph_nojobs[0] = (int) (log( tunemployed_population  / t) * scale);
        if (monthgraph_nojobs[0] < 0)
            monthgraph_nojobs[0] = 0;
        if (monthgraph_nojobs[0] >= h)
            monthgraph_nojobs[0] = h - 1;

        /* percentage scale */
        f = (float) people_pool / (float) (tpopulation / t + people_pool) * (float) h;
        monthgraph_ppool[0] = (int) f;
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
        }
    } else {
        sustain_flag = 0;
    }


    //sustain_flag == 1 means player had a sustainable economy
    //total_evacuated >0 means player evacuated at least some people
    if( !housed_population && !people_pool ){ //no people left
        if( !nobodyHomeDialogShown ){
            std::string message;
            if( sustain_flag == 1 || total_evacuated >0  ){
                message ="";
            } else {
                message = _("You lose.");
            }
            try{
                new Dialog( MSG_DIALOG, "allgone.xml", message );
            } catch(std::exception& e) {
                std::cerr << "Problem with ok_dial_box: " << e.what() << "\n";
                std::ostringstream text;
                text << "ok_dial_box:' allgone.xml" << "' + \"" << message << "\"\n";
                updateMessageText( text.str() );
            }
            nobodyHomeDialogShown = true;
        }
    } else if( nobodyHomeDialogShown ){ //reset flag if there are people
        nobodyHomeDialogShown = false;
    }


    Component* root = this;
    while( root->getParent() ){
        root = root->getParent();
    }

    // Initialisation can not be done in constructor because the SwitchEconomyGraph-Button does not exist then.
    if( switchEconomyGraphButton == NULL ){
        switchEconomyGraphButton = getCheckButton( *root, "SwitchEconomyGraph" );
        switchEconomyGraphText = switchEconomyGraphButton->getCaptionText();
        switchEconomyGraphParagraph = dynamic_cast<Paragraph*>(switchEconomyGraphButton->getCaption());
        redStyle = yellowStyle = normalStyle = switchEconomyGraphParagraph->getStyle();
        yellowStyle.text_color.parse("yellow");
        redStyle.text_color.parse("red");
    }

    // set tab Button colour
    if( switchEconomyGraphParagraph ){
        if( monthgraph_starve[0] > 0 ){ // people are starving: RED
            switchEconomyGraphParagraph->setText(switchEconomyGraphText, redStyle);
        } else if( monthgraph_nojobs[0] > 0 ){ // people are unemployed: YELLOW
            switchEconomyGraphParagraph->setText(switchEconomyGraphText, yellowStyle);
        } else {
            switchEconomyGraphParagraph->setText(switchEconomyGraphText, normalStyle);
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

    Vector2 a, b, p;

#define SUST_BAR_H      5
#define SUST_BAR_GAP_Y  5

	/* draw the starting line */
    a.x = mgX + 38;
    a.y = mgY;
    b.x = mgX + 38;
    b.y = mgY + mgH;
    p.x = mgX;
    p.y = mgY;
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
    painter.drawTexture( labelTextureMIN, p );

	/* import export */
    p.y += SUST_BAR_H + SUST_BAR_GAP_Y ;
    newLen = maxBarLen * sust_port_count / SUST_PORT_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( black );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    painter.drawTexture( labelTexturePRT, p );

	/* money */
    p.y += SUST_BAR_H + SUST_BAR_GAP_Y ;
    newLen = maxBarLen * sust_old_money_count / SUST_MONEY_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( green );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    painter.drawTexture( labelTextureMNY, p );

	/* population */
    p.y += SUST_BAR_H + SUST_BAR_GAP_Y ;
    newLen = maxBarLen * sust_old_population_count / SUST_POP_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( blue );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    painter.drawTexture( labelTexturePOP, p );

	/* tech */
    p.y += SUST_BAR_H + SUST_BAR_GAP_Y ;
    newLen = maxBarLen * sust_old_tech_count / SUST_TECH_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( yellow );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    painter.drawTexture( labelTextureTEC, p );

	/* fire */
    p.y += SUST_BAR_H + SUST_BAR_GAP_Y ;
    newLen = maxBarLen * sust_fire_count / SUST_FIRE_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( red );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    painter.drawTexture( labelTextureFIR, p );
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
    int mgY = 3*border;
    int mgW = getConfig()->monthgraphW;
    int mgH = getConfig()->monthgraphH;

    painter.setFillColor( white );
    painter.fillRectangle( background );

    Vector2 labelPos( 2 * border, border-1 );

    //Draw HistoryLineGraph
    painter.drawTexture( labelTextureEconomy, labelPos );
    Rect2D currentGraph( mgX, mgY, mgX + mgW, mgY + mgH );
    drawHistoryLineGraph( painter, currentGraph );

    //Draw Sustainability Bars
    labelPos.y += 2 * border + mgH;
    painter.drawTexture( labelTextureSustainability, labelPos );
    currentGraph.move( Vector2( 0, 2 * border + mgH ) );
    drawSustBarGraph( painter, currentGraph);

    //Draw FPS-Window
    labelPos.y += 2 * border + mgH;
    painter.drawTexture( labelTextureFPS, labelPos );
    currentGraph.move( Vector2( 0, 2 * border + mgH ) );
    currentGraph.setHeight( mgH/2 );
    drawFPSGraph( painter, currentGraph );
}

IMPLEMENT_COMPONENT_FACTORY(EconomyGraph)

/** @file lincity-ng/EconomyGraph.cpp */

