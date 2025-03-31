/* ---------------------------------------------------------------------- *
 * src/lincity-ng/EconomyGraph.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Wolfgang Becker <uafr@gmx.de>
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "EconomyGraph.hpp"

#include <SDL.h>                           // for SDL_Surface
#include <SDL_ttf.h>                       // for TTF_RenderUTF8_Blended
#include <stdio.h>                         // for sscanf, NULL
#include <stdlib.h>                        // for free, malloc
#include <string.h>                        // for strcmp
#include <cmath>                           // for log
#include <exception>                       // for exception
#include <iostream>                        // for operator<<, basic_ostream
#include <sstream>                         // for basic_stringstream
#include <stdexcept>                       // for runtime_error

#include "Config.hpp"                      // for getConfig, Config
#include "Dialog.hpp"                      // for Dialog, MSG_DIALOG
#include "Util.hpp"                        // for getCheckButton
#include "gui/CheckButton.hpp"             // for CheckButton
#include "gui/Color.hpp"                   // for Color
#include "gui/ComponentFactory.hpp"        // for IMPLEMENT_COMPONENT_FACTORY
#include "gui/FontManager.hpp"             // for FontManager, fontManager
#include "gui/Painter.hpp"                 // for Painter
#include "gui/Paragraph.hpp"               // for Paragraph
#include "gui/Rect2D.hpp"                  // for Rect2D
#include "gui/Style.hpp"                   // for Style
#include "gui/Texture.hpp"                 // for Texture
#include "gui/TextureManager.hpp"          // for TextureManager, texture_ma...
#include "gui/Vector2.hpp"                 // for Vector2
#include "gui/XmlReader.hpp"               // for XmlReader
#include "lincity/engglobs.h"              // for sustain_flag, people_pool
#include "lincity/lin-city.h"              // for GOOD
#include "lincity/lintypes.h"              // for NUMOF_DAYS_IN_MONTH
#include "lincity/stats.h"                 // for tpopulation, tstarving_pop...
#include "lincity/sustainable.h"           // for SUST_FIRE_YEARS_NEEDED
#include "tinygettext/gettext.hpp"         // for _
#include "Game.hpp"

EconomyGraph::EconomyGraph(){
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

void
EconomyGraph::setGame(Game *game) {
  this->game = game;
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

  float scale = (float) mgH / NUMOF_DAYS_IN_MONTH / (5.3 * log(10.));
  float ppoolScale = (float) mgH / NUMOF_DAYS_IN_MONTH;
  auto& history = game->getWorld().stats.history;

  b.y = mgY + mgH;
  for( int i = mgW - 1; i >= 0; i-- ){
    painter.setLineColor( yellow );
    a.x = mgX + mgW - i;
    a.y = mgY + mgH - scale * history.nojobs[i];

    b.x = mgX + mgW - i;
    painter.drawLine( a, b );
    painter.setLineColor( red );
    a.y = mgY + mgH - scale * history.starve[i];
    painter.drawLine( a, b );
  }
  for( int i = mgW - 1; i > 0; i-- ){
    painter.setLineColor( brown );
    a.x = mgX + mgW - i;
    a.y = mgY + mgH - scale * history.pop[i];
    b.x = mgX + mgW - i-1;
    b.y = mgY + mgH - scale * history.pop[i-1];
    painter.drawLine( a, b );
    a.y = mgY + mgH - ppoolScale * history.ppool[i] / history.pop[i];
    b.y = mgY + mgH - ppoolScale * history.ppool[i-1] / history.pop[i-1];
    painter.setLineColor( blue );
    painter.drawLine( a, b );
  }
  painter.clearClipRectangle();


  // set tab Button colour
  Component* root = this;
  while(root->getParent()) root = root->getParent();
  if(switchEconomyGraphButton == NULL) {
    // Initialisation can not be done in constructor because the
    // SwitchEconomyGraph-Button does not exist then.
    switchEconomyGraphButton = getCheckButton( *root, "SwitchEconomyGraph" );
    switchEconomyGraphText = switchEconomyGraphButton->getCaptionText();
    switchEconomyGraphParagraph = dynamic_cast<Paragraph*>(
      switchEconomyGraphButton->getCaption());
    redStyle = yellowStyle = normalStyle = switchEconomyGraphParagraph->getStyle();
    yellowStyle.text_color.parse("yellow");
    redStyle.text_color.parse("red");
  }
  if(history.starve[0] > 0) { // people are starving: RED
    switchEconomyGraphParagraph->setText(switchEconomyGraphText, redStyle);
  } else if(history.nojobs[0] > 0) { // people are unemployed: YELLOW
    switchEconomyGraphParagraph->setText(switchEconomyGraphText, yellowStyle);
  } else {
    switchEconomyGraphParagraph->setText(switchEconomyGraphText, normalStyle);
  }

  setDirty();
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
    auto& sustainability = game->getWorld().stats.sustainability;

	/* ore coal */
    newLen = sustainability.mining_years *
      maxBarLen / SUST_ORE_COAL_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( orange );
    painter.fillRectangle( bar );
    painter.drawTexture( labelTextureMIN, p );

	/* import export */
    p.y += SUST_BAR_H + SUST_BAR_GAP_Y ;
    newLen = sustainability.trade_years * maxBarLen / SUST_PORT_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( black );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    painter.drawTexture( labelTexturePRT, p );

	/* money */
    p.y += SUST_BAR_H + SUST_BAR_GAP_Y ;
    newLen = sustainability.money_years * maxBarLen / SUST_MONEY_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( green );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    painter.drawTexture( labelTextureMNY, p );

	/* population */
    p.y += SUST_BAR_H + SUST_BAR_GAP_Y ;
    newLen = sustainability.population_years *
      maxBarLen / SUST_POP_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( blue );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    painter.drawTexture( labelTexturePOP, p );

	/* tech */
    p.y += SUST_BAR_H + SUST_BAR_GAP_Y ;
    newLen = sustainability.tech_years * maxBarLen / SUST_TECH_YEARS_NEEDED;
    len = 3 + ( ( newLen > maxBarLen ) ? maxBarLen : newLen );
    bar.setWidth( len );
    painter.setFillColor( yellow );
    bar.move( Vector2( 0, SUST_BAR_H + SUST_BAR_GAP_Y ) );
    painter.fillRectangle( bar );
    painter.drawTexture( labelTextureTEC, p );

	/* fire */
    p.y += SUST_BAR_H + SUST_BAR_GAP_Y ;
    newLen = sustainability.fire_years * maxBarLen / SUST_FIRE_YEARS_NEEDED;
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
