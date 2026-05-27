/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>
Copyright (C) 2026 Marc Young <myoung008@gmail.com>

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
 * @file Paragraph.cpp
 */

#include "Paragraph.hpp"

#include <SDL3/SDL.h>                     // for SDL_Surface, SDL_DestroySurface
#include <SDL3_ttf/SDL_ttf.h>             // for TTF_FontHeight, TTF_FontAscent
#include <assert.h>                       // for assert
#include <fmt/base.h>                     // for println
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <cstdio>                         // for stderr
#include <map>                            // for map, operator==
#include <memory>                         // for unique_ptr, allocator
#include <regex>                          // for basic_regex, regex_replace
#include <sstream>                        // for basic_stringstream, operator<<
#include <stdexcept>                      // for runtime_error
#include <utility>                        // for pair

#include "Color.hpp"                      // for Color
#include "ComponentFactory.hpp"           // for IMPLEMENT_COMPONENT_FACTORY
#include "Event.hpp"                      // for Event
#include "FontManager.hpp"                // for FontManager, fontManager
#include "Painter.hpp"                    // for Painter
#include "Texture.hpp"                    // for Texture
#include "TextureManager.hpp"             // for TextureManager, texture_man...
#include "Vector2.hpp"                    // for Vector2
#include "util/gettextutil.hpp"           // for _
#include "util/xmlutil.hpp"               // for unexpectedXmlAttribute, xml...

Paragraph::Paragraph() {
  setFlags(FLAG_RESIZABLE);
}

Paragraph::~Paragraph() { }

void
Paragraph::parse(xmlpp::TextReader& reader)
{
    parse(reader, style);
}

void
Paragraph::parseList(xmlpp::TextReader& reader, const Style& )
{
    // query for "list" style
    std::map<std::string, Style>::iterator i
        = styleRegistry.find("list");
    if(i == styleRegistry.end()) {
        throw std::runtime_error("<li> element used but no"
                " list style defined");
    }

    // add a bullet char at the front of the text
    TextSpan* currentspan = new TextSpan();
    currentspan->style = i->second;
    currentspan->text = " \342\200\242 ";
    textspans.push_back(std::unique_ptr<TextSpan>(currentspan));

    parse(reader, i->second);
}

void
Paragraph::commit_changes(
  std::unique_ptr<TextSpan>& currentspan, bool translatable
) {
  if(!currentspan) return;
  if(translatable) {
    currentspan->text = _(currentspan->text);
  }
  textspans.push_back(std::unique_ptr<TextSpan>(currentspan.release()));
}

void
Paragraph::parse(xmlpp::TextReader& reader, const Style& parentstyle) {
  bool translatable = false;
  style = parentstyle;

  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(style.parseAttribute(reader));
    else if(name == "translatable")
      translatable = xmlParse<bool>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  std::vector<Style> stylestack;
  stylestack.push_back(style);
  stylestack.back().toSpan();

  std::unique_ptr<TextSpan> currentspan;

  if(!reader.is_empty_element() && reader.read())
  while(true) {
    switch(reader.get_node_type()) {
    case xmlpp::TextReader::NodeType::Element: {
      xmlpp::ustring node = reader.get_name();
      if(node != "span" && node != "i" && node != "b" && node != "a") {
        unexpectedXmlElement(reader);
        break;
      }

      commit_changes(currentspan,translatable);

      Style style(stylestack.back());
      if (node == "a") {
        style.text_color.parse("blue");
      } else if(node == "i") {
        style.italic = true;
      } else if(node == "b") {
        style.bold = true;
      }

      while(reader.move_to_next_attribute()) {
        xmlpp::ustring name = reader.get_name();
        xmlpp::ustring value = reader.get_value();
        if(style.parseAttribute(reader));
        else
          unexpectedXmlAttribute(reader);
      }
      reader.move_to_element();

      stylestack.push_back(style);
    } break;
    case xmlpp::TextReader::NodeType::Text: {
      if(!currentspan) {
        currentspan.reset(new TextSpan());
        currentspan->style = stylestack.back();
      }

      std::string p = xmlParse<std::string>(reader.get_value());
      currentspan->text += std::regex_replace(p, std::basic_regex("\\s+"), " ");
    } break;
    case xmlpp::TextReader::NodeType::EndElement: {
      commit_changes(currentspan,translatable);
      stylestack.pop_back();
      if(stylestack.empty()) goto done;

      std::string node = reader.get_name();
      if(node != "span" && node != "b" && node != "i" && node != "a") {
        fmt::println(stderr, "error: unexpected xml end element {:?}", node);
        assert(false);
      }
    } break;
    }
    reader.read();
  }
  done:;
}

/**
 * Reflows the text and renders it onto a texture
 * Cleaning this big code up a bit more is always nice. However be very careful
 * when doing so and test it alot, as the code very easily breaks...
 *
 * TODO:
 *   - properly handle whitespace at the end of a span
 *   - should use pixel coordinates instead of display coordinates
 *   - no span alignment --> use TableLayout instead
 *   - option for no auto-flow
 *   - handle newline characters as manual line breaks
 */
void
Paragraph::resize(float width, float height) {
    // y coordinates for all the lines
    std::vector<int> ycoords;
    // surfaces of all the lines rendered
    std::vector<SDL_Surface*> lineimages;
    // surfaces for the current line
    std::vector<SDL_Surface*> spanimages;
    std::vector<float> spanxoffset;
    std::vector<LinkRectangle> linerectangles;
    std::vector<int> spanbaselines;
    int lineheight = 0;
    int baseline = 0;


    TextSpans::iterator i = textspans.begin();
    if(i == textspans.end()) {
      texture.reset();
      this->width = 0;
      this->height = 0;
      return;
    }

    const TextSpan* span = i->get();
    const std::string* text = &(span->text);
    const Vector2 scale = getScale();
    TTF_Font* font = fontManager->getFont(span->style, scale);
    std::string::size_type p = 0;
    std::string::size_type linestart = 0;
    lineheight = TTF_GetFontHeight(font) / scale.y;
    // string that should be rendered next
    std::string line;
    // current rendering position
    Vector2 pos;
    while(1) {
        std::string::size_type lastp = p;
        if( (*text) [p] == ' ')
        {
            // we don't need the space at the beginning of the line
            if(p-linestart != 0 || pos.x != 0)
            {   line += ' ';}
            else
            {
                lastp++;
                linestart++;
            }
            ++p;
        }

        // take a word
        for( ; p < text->size() &&
            !( ((*text) [p] == ' ') /*|| ((*text) [p] == '\t')*/ ); ++p)
        {   line += (*text) [p];}

        // check line size...
        int render_width, render_height;
        TTF_GetStringSize(font, line.c_str(), 0, &render_width, &render_height);

        bool render = false;
        bool linefeed = false;
        // we need a linefeed if width isn't enough for current span
        if(width > 0 && pos.x + render_width / scale.x >= width - style.margin_left - style.margin_right)
        {
            render = true;
            linefeed = true;
            // we have to leave out the last word (which made it too width)
            if(lastp-linestart > 0 || pos.x != 0) {
                line = std::string(*text, linestart, lastp-linestart);
                // set new linestart and set p back
                p = lastp;
            }
            linestart = p;
        }

        /* span is over, so we need to render now (and if it was the last span,
         * we need a linefeed too)
         */
        if(p >= text->size())
        {
            render = true;
            ++i;
            if(i == textspans.end())
            {   linefeed = true;}
        }

        if(render && line != "")
        {
            if(TTF_GetFontHeight(font) / scale.y > lineheight)
            {
                lineheight = TTF_GetFontHeight(font) / scale.y;
                baseline = TTF_GetFontAscent(font) / scale.y;
            }

            // render span
            //printf("Rendering: '%s'.\n", line.c_str());
            SDL_Surface* spansurface = TTF_RenderText_Blended(font,
                    line.c_str(), 0, span->style.text_color.getSDLColor());
            if(spansurface == 0) {
                std::stringstream msg;
                msg << "Error rendering text: " << SDL_GetError();
                throw std::runtime_error(msg.str());
            }
            SDL_SetSurfaceAlphaMod(spansurface, 255); // is this needed?
            //remember individual margins of spans
            float xoffset;
            bool new_column = false;
            if(span->style.alignment == Style::ALIGN_LEFT)
            {
                xoffset = span->style.margin_left;
                new_column = (xoffset!=0);
            }
            else if(span->style.alignment == Style::ALIGN_CENTER)
            {
                new_column = (span->style.margin_left!=0);
                if (new_column)
                {   xoffset = (width + span->style.margin_left - span->style.margin_right - spansurface->w / scale.x)/2;}
                else
                {   xoffset = 0;}
            }
            else
            {
                new_column = true; //always new column for right adjustment
                xoffset = (width - spansurface->w / scale.x - span->style.margin_right);
            }
            float yoffset = span->style.margin_top;
            if (new_column)
            {   pos.x = xoffset;}//fixed columns
            else
            {   pos.x += xoffset;}
            pos.y += yoffset;
            spanxoffset.push_back(pos.x);
            spanimages.push_back(spansurface);
            spanbaselines.push_back(TTF_GetFontAscent(font) / scale.y);

            // remember span position if it is a link
            if(span->style.href != "") {
                LinkRectangle link;
                link.rect = Rect2D (pos.x , pos.y,
                                    pos.x + spansurface->w / scale.x,
                                    pos.y + spansurface->h / scale.y);
                link.span = span;
                linerectangles.push_back(link);
            }


            pos.x += spansurface->w / scale.x;
            line = "";
        }

        // linefeed: compose all span images to line image
        if(linefeed) {
            // compose all spanimages into a line surface
            if(spanimages.size() == 1) {
                lineimages.push_back(spanimages.back());
            } else {
                SDL_Surface* lineimage = SDL_CreateSurface(pos.x * scale.x,
                        lineheight * scale.y, SDL_PIXELFORMAT_ABGR8888);
                if(lineimage == 0) {
                    throw std::runtime_error(
                            "Out of memory when composing line image");
                }
                SDL_SetSurfaceAlphaMod(lineimage, 255);

                SDL_Rect rect;
                for(size_t i = 0; i < spanimages.size(); ++i) {
                    rect.x = spanxoffset[i] * scale.x;
                    rect.y = (baseline - spanbaselines[i] + textspans[i]->style.margin_top) * scale.y;
                    if(rect.y < 0)
                    {   rect.y = 0;}

                    SDL_BlitSurface(spanimages[i], 0, lineimage, &rect);
                    SDL_DestroySurface(spanimages[i]);
                }
                lineimages.push_back(lineimage);
            }
            spanbaselines.clear();
            spanimages.clear();
            spanxoffset.clear();

            // adjust link rectangles for alignment and add them to the list
            float xoffset;
            if(style.alignment == Style::ALIGN_LEFT) {
                xoffset = style.margin_left;
            } else if(style.alignment == Style::ALIGN_CENTER) {
                xoffset = (width + style.margin_left - style.margin_right - lineimages.back()->w / scale.x)/2;
            } else {
                xoffset = (width - lineimages.back()->w / scale.x  - style.margin_right);
            }
            for(std::vector<LinkRectangle>::iterator i =linerectangles.begin();
                i != linerectangles.end(); ++i) {
                i->rect.move(Vector2(xoffset, style.margin_top));
                linkrectangles.push_back(*i);
            }
            linerectangles.clear();

            line = "";
            pos.x = 0;

            ycoords.push_back(static_cast<int> (pos.y + style.margin_top));
            pos.y += lineheight;

            lineheight = TTF_GetFontHeight(font) / scale.y;
            baseline = TTF_GetFontAscent(font) / scale.y;
        }

        // advance to next span if necessary
        if(p >= text->size())
        {
            if(i == textspans.end())
            {   break;}
            span = i->get();
            text = &(span->text);
            font = fontManager->getFont(span->style, scale);
            linestart = p = 0;
            line = "";
        }
    }

    height = pos.y + style.margin_top + style.margin_bottom;
    if(height < style.min_height)
    {   height = style.min_height;}

    // check height defined in style
    if(height == 0) {
        this->width = this->height = 0;
        for(std::vector<SDL_Surface*>::iterator i = lineimages.begin();
                i != lineimages.end(); ++i)
            SDL_DestroySurface(*i);
        return;
    }

    /* Step2: compose all lines to the final image */
    if(width < 0) {
        width = lineimages[0]->w / scale.x;
    }
    SDL_Surface* result = SDL_CreateSurface((int)(width * scale.x), (int)(height * scale.y),
                                            SDL_PIXELFORMAT_ABGR8888);
    if(result == 0) {
        throw std::runtime_error("Out of memory when creating text image");
    }
    if(!result->w || !result->h) {
      SDL_DestroySurface(result);
      texture.reset();
      this->width = 0;
      this->height = 0;
      return;
    }
    //apply margins of paragraph
    for(size_t i = 0; i < lineimages.size(); ++i) {
        SDL_Rect rect;

        if(style.alignment == Style::ALIGN_LEFT) {
            rect.x = style.margin_left * scale.x;
        } else if(style.alignment == Style::ALIGN_CENTER) {
            rect.x = ((width + style.margin_left - style.margin_right) * scale.x - lineimages[i]->w) / 2;
        } else {
            rect.x = (width - style.margin_right) * scale.x - lineimages[i]->w;
        }
        rect.y = ycoords[i] * scale.y;
        SDL_BlitSurface(lineimages[i], 0, result, &rect);
        SDL_DestroySurface(lineimages[i]);
    }
    SDL_Surface* surface = SDL_ConvertSurface(result, SDL_PIXELFORMAT_RGBA8888);
    SDL_DestroySurface(result);
    if(surface == NULL)
    {   throw std::runtime_error("Out of memory when creating text image");}

    texture.reset(texture_manager->create(surface));
    SDL_DestroySurface(surface);
    this->width = width;
    this->height = height;

    setDirty();
}

void
Paragraph::draw(Painter& painter) {
  if(texture)
    painter.drawTexture(texture.get(), Vector2(0, 0));
}

void
Paragraph::event(const Event& event)
{
  switch(event.type) {
  case Event::MOUSEMOTION:
  case Event::MOUSEBUTTONDOWN: {
    if(!event.inside) break;
    for(LinkRectangles::iterator i = linkrectangles.begin();
      i != linkrectangles.end(); ++i
    ) {
      if(i->rect.inside(event.mousepos)) {
        if(event.type == Event::MOUSEMOTION) {
          // TODO change mouse cursor
        } else if(event.type == Event::MOUSEBUTTONDOWN) {
          linkClicked(this, i->span->style.href);
        }
      }
    }
  } break;
  }

  Component::event(event);
}

void
Paragraph::setText(const std::string& newtext)
{
    Style spanStyle = style;
    spanStyle.toSpan();
    setText(newtext, spanStyle);
}

std::string Paragraph::getText() const
{
  std::string t;
  for(TextSpans::const_iterator i = textspans.begin(); i != textspans.end(); ++i)
    t+=(*i)->text;
  return t;
}

void
Paragraph::setText(const std::string& newtext, const Style& style)
{
/*
    for(TextSpans::iterator i = textspans.begin(); i != textspans.end(); ++i)
        delete *i;
*/
    textspans.clear();

    size_t span_end = newtext.find_first_of('\t',0);
    if(span_end != newtext.npos) // we have a tab
    {
        int tabcount = 0;
        std::string mytext = newtext;
        std::string spantext;
        while(mytext.size())
        {
            spantext = mytext.substr(0,span_end);//skip the first tab
            mytext.erase(0,span_end);//drop first span
            mytext.erase(0,1);//kill the first tab
            span_end = mytext.find_first_of('\t',0); // next tab
            TextSpan* span = new TextSpan();
            span->style = style;
            span->style.toSpan();
            span->text = spantext;
            textspans.push_back(std::unique_ptr<TextSpan>(span));
            ++tabcount;
        }
        if (tabcount == 2)
        {
            textspans[0]->style.alignment = Style::ALIGN_LEFT;
            textspans[1]->style.alignment = Style::ALIGN_RIGHT;
        }
        else if (tabcount == 3)
        {
            textspans[0]->style.alignment = Style::ALIGN_LEFT;
            textspans[1]->style.alignment = Style::ALIGN_RIGHT;
            textspans[1]->style.margin_right = 65;
            textspans[2]->style.alignment = Style::ALIGN_RIGHT;
        }
    }
    else // simple string to parse
    {
        TextSpan* span = new TextSpan();
        span->style = style;
        span->style.toSpan();
        span->text = newtext;
        textspans.push_back(std::unique_ptr<TextSpan>(span));
    }

    float oldWidth = width;
    float oldHeight = height;
    // rerender text
    resize(width, height);
    // eventually trigger resize/relayout of parent component
    if(width == 0 || width != oldWidth || height == 0 || height != oldHeight)
    {
        if(getParent())
        {   getParent()->reLayout();}
    }
}

IMPLEMENT_COMPONENT_FACTORY(Paragraph)


/** @file gui/Paragraph.cpp */
