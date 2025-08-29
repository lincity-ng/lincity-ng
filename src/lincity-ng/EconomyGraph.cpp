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

#include <SDL.h>                          // for SDL_Surface
#include <SDL_ttf.h>                      // for TTF_RenderUTF8_Blended, TTF...
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <algorithm>                      // for min, max
#include <cassert>                        // for assert
#include <cmath>                          // for log, sqrt
#include <cstddef>                        // for NULL
#include <deque>                          // for deque

#include "Game.hpp"                       // for Game
#include "Util.hpp"                       // for getCheckButton
#include "gui/CheckButton.hpp"            // for CheckButton
#include "gui/Color.hpp"                  // for Color
#include "gui/ComponentFactory.hpp"       // for IMPLEMENT_COMPONENT_FACTORY
#include "gui/FontManager.hpp"            // for FontManager, fontManager
#include "gui/Painter.hpp"                // for Painter
#include "gui/Paragraph.hpp"              // for Paragraph
#include "gui/Rect2D.hpp"                 // for Rect2D
#include "gui/Style.hpp"                  // for Style
#include "gui/Texture.hpp"                // for Texture
#include "gui/TextureManager.hpp"         // for TextureManager, texture_man...
#include "gui/Vector2.hpp"                // for Vector2
#include "lincity/lintypes.hpp"           // for NUMOF_DAYS_IN_MONTH
#include "lincity/stats.hpp"              // for Stats
#include "lincity/sustainable.hpp"        // for SUST_FIRE_YEARS_NEEDED, SUS...
#include "lincity/world.hpp"              // for World
#include "util/gettextutil.hpp"           // for _
#include "util/xmlutil.hpp"               // for xmlParse, unexpectedXmlAttr...

EconomyGraph::EconomyGraph() {
    labelTextureMIN = 0;
    labelTexturePRT = 0;
    labelTextureMNY = 0;
    labelTexturePOP = 0;
    labelTextureTEC = 0;
    labelTextureFIR = 0;
    labelTextureEconomy = 0;
    labelTextureSustainability = 0;
    labelTextureFPS = 0;

    switchEconomyGraphButton = NULL;
}

EconomyGraph::~EconomyGraph(){
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

void
EconomyGraph::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(name == "width")
      width = xmlParse<float>(value);
    else if(name == "height")
      height = xmlParse<float>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  //Generate Labels for Sustainability Graph
  Style labelStyle;
  labelStyle.font_family = "sans";
  labelStyle.font_size = 10;
  TTF_Font* font = fontManager->getFont( labelStyle );
  SDL_Surface* labelXXX;
  /*  MIN=Mining, PRT=Import/export from port,
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

void
EconomyGraph::newFPS(int frame) {
  fps.push_front(frame);
  fps.resize(getWidth() - border * 2);

  setDirty();
}

void
EconomyGraph::drawHistoryLineGraph(Painter& painter, Rect2D space) {
  Color red, yellow, blue, brown, grey;
  red.parse("red");
  yellow.parse("yellow");
  blue.parse("blue");
  brown.parse("brown");
  grey.parse("#A9A9A9FF");

  painter.setFillColor(grey);
  painter.fillRectangle(space);

  auto& history = game->getWorld().stats.history;
  const int w = (int)space.getWidth();
  if(history.pop.size() < w) history.pop.resize(w);
  if(history.ppool.size() < w) history.ppool.resize(w);
  if(history.nojobs.size() < w) history.nojobs.resize(w);
  if(history.starve.size() < w) history.starve.resize(w);

  // Half needs to be (.0,.0) to avoid gaps in the line from
  // implementation-dependent tie-breaking. If half is (.5,.5) then lines will
  // traverse exactly half way through the start/end pixels, and sometimes these
  // "half-way" pixels are not draw as we want them to be. Setting half to
  // (.0,.0) makes the lines traverse from one side of the pixel to the other,
  // thus making sure the pixel is drawn.
  const Vector2 half(.0, .0);
  const float popScale0 = -log(100. * NUMOF_DAYS_IN_MONTH);
  const float popScale1 = 1/log(1000.);
  int pop = 0, popPrev;
  float val, valP;
  for(int i = 0; i < w; i++) {
    popPrev = pop;
    pop = history.pop[i];

    if(!pop) continue;

    val = 2 * sqrt((float)history.nojobs[i] / pop);
    painter.setFillColor(yellow);
    painter.fillRectangle(Rect2D(
      space.p2 - Vector2(i+1, space.getHeight() * std::min(1.f, val)),
      space.p2 - Vector2(i, 0)));

    val = 2 * sqrt((float)history.starve[i] / pop);
    painter.setFillColor(red);
    painter.fillRectangle(Rect2D(
      space.p2 - Vector2(i+1, space.getHeight() * std::min(1.f, val)),
      space.p2 - Vector2(i, 0)));

    // glEnable(GL_LINE_SMOOTH);
    val = std::max(0.f, log((float)pop) + popScale0) * popScale1;
    valP = !popPrev ? val :
      std::max(0.f, log((float)popPrev) + popScale0) * popScale1;
    painter.setLineColor(brown);
    painter.drawLine(
      space.p2 - half
        - Vector2(i, (space.getHeight()-1) * std::min(1.f, val)),
      space.p2 - half
        - Vector2(i+1, (space.getHeight()-1) * std::min(1.f, valP)));

    val = 2 * sqrt((float)history.ppool[i] / pop);
    valP = !popPrev ? val :
      2 * sqrt((float)history.ppool[i - 1] / popPrev);
    painter.setLineColor(blue);
    painter.drawLine(
      space.p2 - half
        - Vector2(i, (space.getHeight()-1) * std::min(1.f, val)),
      space.p2 - half
        - Vector2(i+1, (space.getHeight()-1) * std::min(1.f, valP)));

    // glDisable(GL_LINE_SMOOTH);
  }
  // painter.clearClipRectangle();


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
}

void
EconomyGraph::drawSustBarGraph(Painter& painter, Rect2D space) {
  Color grey,yellow,orange,black,green,blue,red;
  grey.parse( "#A9A9A9FF" );
  yellow.parse( "yellow" );
  orange.parse( "orange" );
  black.parse( "black" );
  green.parse( "green" );
  blue.parse( "blue" );
  red.parse( "red" );

  painter.setFillColor(grey);
  painter.fillRectangle(space);

  const auto& sustainability = game->getWorld().stats.sustainability;
  const float startLine = space.p1.x + sustBarStart;
  const float barLeft = startLine - sustBarStub;
  const float lenMax = space.getWidth() - sustBarStart;
  float y = space.p1.y;
  float len;

  /* draw the starting line */
  painter.setLineColor(yellow);
  painter.drawLine(
    Vector2(startLine, space.p1.y),
    Vector2(startLine, space.p2.y));

  // mining
  len = sustainability.mining_years * lenMax / SUST_ORE_COAL_YEARS_NEEDED;
  painter.setFillColor(orange);
  painter.fillRectangle(Rect2D(
    Vector2(barLeft, y + sustBarVOffset),
    Vector2(std::min(startLine + len, space.p2.x),
      y + sustBarVOffset + sustBarHeight)));
  painter.drawTexture(labelTextureMIN,
    Vector2(space.p1.x, y + sustLabelVOffset));
  y += sustBarSpace;

  // trade
  len = sustainability.trade_years * lenMax / SUST_PORT_YEARS_NEEDED;
  painter.setFillColor(black);
  painter.fillRectangle(Rect2D(
    Vector2(barLeft, y + sustBarVOffset),
    Vector2(std::min(startLine + len, space.p2.x),
      y + sustBarVOffset + sustBarHeight)));
  painter.drawTexture(labelTexturePRT,
    Vector2(space.p1.x, y + sustLabelVOffset));
  y += sustBarSpace;

  // money
  len = sustainability.money_years * lenMax / SUST_MONEY_YEARS_NEEDED;
  painter.setFillColor(green);
  painter.fillRectangle(Rect2D(
    Vector2(barLeft, y + sustBarVOffset),
    Vector2(std::min(startLine + len, space.p2.x),
      y + sustBarVOffset + sustBarHeight)));
  painter.drawTexture(labelTextureMNY,
    Vector2(space.p1.x, y + sustLabelVOffset));
  y += sustBarSpace;

  // population
  len = sustainability.population_years * lenMax / SUST_POP_YEARS_NEEDED;
  painter.setFillColor(blue);
  painter.fillRectangle(Rect2D(
    Vector2(barLeft, y + sustBarVOffset),
    Vector2(std::min(startLine + len, space.p2.x),
      y + sustBarVOffset + sustBarHeight)));
  painter.drawTexture(labelTexturePOP,
    Vector2(space.p1.x, y + sustLabelVOffset));
  y += sustBarSpace;

  // tech
  len = sustainability.tech_years * lenMax / SUST_TECH_YEARS_NEEDED;
  painter.setFillColor(yellow);
  painter.fillRectangle(Rect2D(
    Vector2(barLeft, y + sustBarVOffset),
    Vector2(std::min(startLine + len, space.p2.x),
      y + sustBarVOffset + sustBarHeight)));
  painter.drawTexture(labelTextureTEC,
    Vector2(space.p1.x, y + sustLabelVOffset));
  y += sustBarSpace;

  // fire
  len = sustainability.fire_years * lenMax / SUST_FIRE_YEARS_NEEDED;
  painter.setFillColor(red);
  painter.fillRectangle(Rect2D(
    Vector2(barLeft, y + sustBarVOffset),
    Vector2(std::min(startLine + len, space.p2.x),
      y + sustBarVOffset + sustBarHeight)));
  painter.drawTexture(labelTextureFIR,
    Vector2(space.p1.x, y + sustLabelVOffset));
  y += sustBarSpace;

  assert(y == space.p2.y);
}


void
EconomyGraph::drawFPSGraph(Painter& painter, Rect2D space) {
  Color grey, blue;
  blue.parse("blue");
  grey.parse("#A9A9A9FF");
  // painter.setClipRectangle(space);

  painter.setFillColor(grey);
  painter.fillRectangle(space);

  painter.setFillColor(blue);
  for(int i = 0; i < fps.size(); i++) {
    assert(i < space.getWidth());
    float val = std::min(fps[i] * space.getHeight() / 100, space.getHeight());
    painter.fillRectangle(Rect2D(
      space.p2 - Vector2(i+1, val),
      space.p2 - Vector2(i, 0)
    ));
  }

  // painter.clearClipRectangle();
}

void
EconomyGraph::draw(Painter& painter) {
  Color white;
  white.parse("white");
  Rect2D background(0, 0, getWidth(), getHeight());
  painter.setFillColor( white );
  painter.fillRectangle( background );

  // flex eco graph height
  int ecoHeight = getHeight()
    - border - border
    - headingVSpace - headingVSpace - headingVSpace
    - sustHeight - fpsHeight;

  int y = border;

  //Draw HistoryLineGraph
  painter.drawTexture(labelTextureEconomy,
    Vector2(border, y + headingVOffset));
  y += headingVSpace;
  drawHistoryLineGraph(painter,
    Rect2D(border, y, getWidth() - border, y + ecoHeight));
  y += ecoHeight;

  //Draw Sustainability Bars
  painter.drawTexture(labelTextureSustainability,
    Vector2(border, y + headingVOffset));
  y += headingVSpace;
  drawSustBarGraph(painter,
    Rect2D(border, y, getWidth() - border, y + sustHeight));
  y += sustHeight;

  //Draw FPS-Window
  painter.drawTexture(labelTextureFPS,
    Vector2(border, y + headingVOffset));
  y += headingVSpace;
  drawFPSGraph(painter,
    Rect2D(border, y, getWidth() - border, y + fpsHeight));
  y += fpsHeight;

  y += border;
  assert(y == getHeight());
}

IMPLEMENT_COMPONENT_FACTORY(EconomyGraph)

/*
Drawing lines is really weird. For some reason, they display on a different
pixel than the coordinates indicate.
A vertical line will display one pixel to the left of the x coordinate.
A horizontal line will display one pixel above the y coordinate.

    coord    |   display
-------------+-------------
(1,1)->(1,5) | (0,1)->(0,4)
(1,1)->(5,1) | (1,0)->(4,0)
*/

/** @file lincity-ng/EconomyGraph.cpp */
