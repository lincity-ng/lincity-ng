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
 * @file Paragraph.cpp
 */

#include <config.h>

#include "Paragraph.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ctype.h>
#include <float.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "Event.hpp"
#include "FontManager.hpp"
#include "TextureManager.hpp"
#include "Painter.hpp"
#include "XmlReader.hpp"
#include "ComponentFactory.hpp"
#include "Document.hpp"
#include "tinygettext/tinygettext.hpp"

Paragraph::Paragraph()
    : texture(0)
{
    setFlags(FLAG_RESIZABLE);
}

Paragraph::~Paragraph()
{
    for(TextSpans::iterator i = textspans.begin(); i != textspans.end(); ++i)
        delete *i;
    delete texture;
}

void
Paragraph::parse(XmlReader& reader)
{
    parse(reader, style);
}

void
Paragraph::parseList(XmlReader& reader, const Style& )
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
    textspans.push_back(currentspan);

    parse(reader, i->second);
}

void
Paragraph::parse(XmlReader& reader, const Style& parentstyle)
{
    bool translatable = false;

    style = parentstyle;

    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(style.parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "translatable") == 0) {
            translatable = true;
        } else {
            std::cerr << "Skipping unknown attribut '" << attribute << "'.\n";
        }
    }

    std::vector<Style> stylestack;
    stylestack.push_back(style);
    stylestack.back().toSpan();

    TextSpan* currentspan = 0;

    try {
        std::string currenthref;
        int depth = reader.getDepth();
        while(reader.read() && reader.getDepth() > depth) {
            if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
                std::string node((const char*) reader.getName());
                if(node == "span" || node == "i" || node == "b"
                        || node == "a") {
                    if(currentspan != 0) {
                        if(translatable) {
                            currentspan->text
                                = GUI_TRANSLATE(currentspan->text);
                        }
                        textspans.push_back(currentspan);
                        //std::cout << "new span: " << currentspan->text << std::endl;
                        currentspan = 0;
                    }

                    Style style(stylestack.back());
                    if (node == "a") {
                        style.text_color.parse("blue");
                    } else if(node == "i") {
                        style.italic = true;
                    } else if(node == "b") {
                        style.bold = true;
                    }

                    currenthref = "";
                    XmlReader::AttributeIterator iter(reader);
                    while(iter.next()) {
                        const char* attribute = (const char*) iter.getName();
                        const char* value = (const char*) iter.getValue();
                        if(style.parseAttribute(attribute, value))
                            continue;
                        else if(strcmp(attribute, "href") == 0) {
                            currenthref = value;
                        } else {
                            std::cerr << "Unknown attribute '" << attribute
                                << "' in textspan node.\n";
                        }
                    }
                    style.parseAttributes(reader);
                    // TODO parse style attributes...
                    stylestack.push_back(style);
                } else {
                    std::cerr << "Skipping unknown node '" << node << "'.\n";
                    reader.nextNode();
                }
            } else if(reader.getNodeType() == XML_READER_TYPE_TEXT) {
                if(currentspan == 0) {
                    currentspan = new TextSpan();
                    currentspan->style = stylestack.back();
                }

                const char* p = (const char*) reader.getValue();
                // skip trailing spaces...
                while(*p != 0 && isspace(static_cast<unsigned char>(*p)))
                    ++p;

                bool lastspace = false;
                for( ; *p != 0; ++p) {
                    if(isspace(static_cast<unsigned char>(*p))) {
                        if(!lastspace) {
                            lastspace = true;
                            currentspan->text += ' ';
                        }
                    } else {
                        lastspace = false;
                        currentspan->text += *p;
                        //std::cout << "growing span: " << currentspan->text << std::endl;
                    }
                }
            } else if(reader.getNodeType() == XML_READER_TYPE_END_ELEMENT) {
                std::string node((const char*) reader.getName());
                if(node == "span" || node == "b" || node == "i"
                        || node == "a") {
                    if(currentspan != 0) {
                        if(translatable) {
                            currentspan->text
                                = GUI_TRANSLATE(currentspan->text);
                        }
                        textspans.push_back(currentspan);
                        //std::cout << "add end span: " << currentspan->text << std::endl;
                        currentspan = 0;
                    }
                    stylestack.pop_back();
                } else {
                    std::cerr << "Internal error: unknown node end: '" <<
                        node << "'.\n";
                }
            }
        }

        if(currentspan != 0) {
            if(translatable) {
                currentspan->text
                    = GUI_TRANSLATE(currentspan->text);
            }
            //std::cout << "completed span: " << currentspan->text << std::endl;
            textspans.push_back(currentspan);
            currentspan = 0; //added CK
        }
    } catch(...) {
        if(currentspan != 0)
            delete currentspan;
        throw;
    }
}

/**
 * Reflows the text and renders it onto a texture
 * Cleaning this big code up a bit more is always nice. However be very careful
 * when doing so and test it alot, as the code very easily breaks...
 */
void
Paragraph::resize(float width, float height)
{
    // free old texture
    if(texture)
    {
        delete texture;
        texture = 0;
    }

    if(width == 0)
    {
        this->width = 0;
        this->height = 0;
        texture = 0;
        return;
    }

    // y coordinates for all the lines
    std::vector<int> ycoords;
    // surfaces of all the lines rendered
    std::vector<SDL_Surface*> lineimages;
    // surfaces for the current line
    std::vector<SDL_Surface*> spanimages;
    std::vector<float> spanxoffset, spanyoffset;
    std::vector<LinkRectangle> linerectangles;
    std::vector<int> spanbaselines;
    int lineheight = 0;
    int baseline = 0;

    if(textspans.empty()) {
        // no need to render anything if there are no spans
        this->width = 0;
        this->height = 0;
        texture = 0;
        return;
    }

    TextSpans::iterator i = textspans.begin();

    const TextSpan* span = *i;
    const std::string* text = &(span->text);
    TTF_Font* font = fontManager->getFont(span->style);
    std::string::size_type p = 0;
    std::string::size_type linestart = 0;
    lineheight = TTF_FontHeight(font);
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
        TTF_SizeUTF8(font, line.c_str(), &render_width, &render_height);

        bool render = false;
        bool linefeed = false;
        // we need a linefeed if width isn't enough for current span
        if(width > 0 && pos.x + render_width >= width - style.margin_left - style.margin_right)
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
            if(TTF_FontHeight(font) > lineheight)
            {
                lineheight = TTF_FontHeight(font);
                baseline = TTF_FontAscent(font);
            }

            // render span
            //printf("Rendering: '%s'.\n", line.c_str());
            SDL_Surface* spansurface = TTF_RenderUTF8_Blended(font,
                    line.c_str(), span->style.text_color.getSDLColor());
            if(spansurface == 0) {
                std::stringstream msg;
                msg << "Error rendering text: " << SDL_GetError();
                throw std::runtime_error(msg.str());
            }
            SDL_SetAlpha(spansurface, 0, 0);
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
                {   xoffset = (width + span->style.margin_left - span->style.margin_right - spansurface->w)/2;}
                else
                {   xoffset = 0;}
            }
            else
            {
                new_column = true; //always new column for right adjustment
                xoffset = (width - spansurface->w - span->style.margin_right);
            }
            float yoffset = span->style.margin_top;
            if (new_column)
            {   pos.x = xoffset;}//fixed columns
            else
            {   pos.x += xoffset;}
            pos.y += yoffset;
            spanxoffset.push_back(pos.x);
            spanyoffset.push_back(pos.y);
            spanimages.push_back(spansurface);
            spanbaselines.push_back(TTF_FontAscent(font));

            // remember span position if it is a link
            if(span->style.href != "") {
                LinkRectangle link;
                link.rect = Rect2D (pos.x , pos.y,
                                    pos.x + spansurface->w,
                                    pos.y + spansurface->h);
                link.span = span;
                linerectangles.push_back(link);
            }


            pos.x += spansurface->w;
            line = "";
        }

        // linefeed: compose all span images to line image
        if(linefeed) {
            // compose all spanimages into a line surface
            if(spanimages.size() == 1) {
                lineimages.push_back(spanimages.back());
            } else {
                SDL_Surface* lineimage = SDL_CreateRGBSurface(0, (int) pos.x,
                        (int) lineheight, 32,
                        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
                if(lineimage == 0) {
                    throw std::runtime_error(
                            "Out of memory when composing line image");
                }
                SDL_SetAlpha(lineimage, 0, 0);

                //Sint16 x = 0;
                SDL_Rect rect;
                for(size_t i = 0; i < spanimages.size(); ++i) {
                    rect.x = (Sint16) spanxoffset[i];
                    rect.y = baseline - spanbaselines[i] + (Sint16)spanyoffset[i];
                    if(rect.y < 0)
                    {   rect.y = 0;}

                    SDL_BlitSurface(spanimages[i], 0, lineimage, &rect);
                    //x = /*spanimages[i]->w +*/ (Sint16) spanxoffset[i];
                    //CK: see "pos.x += spansurface->w;"

                    SDL_FreeSurface(spanimages[i]);
                }
                lineimages.push_back(lineimage);
            }
            spanbaselines.clear();
            spanimages.clear();

            // adjust link rectangles for alignment and add them to the list
            float xoffset;
            if(span->style.alignment == Style::ALIGN_LEFT) {
                xoffset = span->style.margin_left;
            } else if(span->style.alignment == Style::ALIGN_CENTER) {
                xoffset = (width + span->style.margin_left - span->style.margin_right - lineimages.back()->w)/2;
            } else {
                xoffset = (width - lineimages.back()->w  - span->style.margin_right);
            }
            for(std::vector<LinkRectangle>::iterator i =linerectangles.begin();
                i != linerectangles.end(); ++i) {
                i->rect.move(Vector2(xoffset, span->style.margin_top));
                linkrectangles.push_back(*i);
            }
            linerectangles.clear();

            line = "";
            pos.x = 0;

            ycoords.push_back(static_cast<int> (pos.y + style.margin_top));
            pos.y += lineheight;

            lineheight = TTF_FontHeight(font);
            baseline = TTF_FontAscent(font);
        }

        // advance to next span if necessary
        if(p >= text->size())
        {
            if(i == textspans.end())
            {   break;}
            span = *i;
            text = &(span->text);
            font = fontManager->getFont(span->style);
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
            SDL_FreeSurface(*i);
        return;
    }

    /* Step2: compose all lines to the final image */
    if(width < 0) {
        width = lineimages[0]->w;
    }
    SDL_Surface* result = SDL_CreateRGBSurface(0, (int) width, (int) height,
            32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    if(result == 0) {
        throw std::runtime_error("Out of memory when creating text image");
    }
    //apply margins of paragraph
    for(size_t i = 0; i < lineimages.size(); ++i) {
        SDL_Rect rect;

        if(style.alignment == Style::ALIGN_LEFT) {
            rect.x = (Sint16) style.margin_left;
        } else if(style.alignment == Style::ALIGN_CENTER) {
            rect.x = (Sint16) (width + style.margin_left - style.margin_right - lineimages[i]->w) / 2;
        } else {
            rect.x = (Sint16) (width - lineimages[i]->w - style.margin_right);
        }
        rect.y = (Sint16) ycoords[i];
        SDL_BlitSurface(lineimages[i], 0, result, &rect);
        SDL_FreeSurface(lineimages[i]);
    }
    SDL_Surface* surface = SDL_DisplayFormatAlpha(result);
    SDL_FreeSurface(result);
    if(surface == 0)
    {   throw std::runtime_error("Out of memory when creating text image(d)");}

    texture = texture_manager->create(surface);
    this->width = width;
    this->height = height;

    setDirty();
}

void
Paragraph::draw(Painter& painter)
{
    if(!texture)
    {   return;}
    painter.drawTexture(texture, Vector2(0, 0));
}

void
Paragraph::event(const Event& event)
{
    if(event.type != Event::MOUSEMOTION &&
       event.type != Event::MOUSEBUTTONDOWN)
        return;
    if(!event.inside)
        return;
    for(LinkRectangles::iterator i = linkrectangles.begin();
        i != linkrectangles.end(); ++i) {
        if(i->rect.inside(event.mousepos)) {
            if(event.type == Event::MOUSEMOTION) {
                // TODO change mouse cursor
            } else if(event.type == Event::MOUSEBUTTONDOWN
                    && event.mousebutton != SDL_BUTTON_WHEELUP
                    && event.mousebutton != SDL_BUTTON_WHEELDOWN) {
                linkClicked(this, i->span->style.href);
            }
        }
    }
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
            textspans.push_back(span);
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
        textspans.push_back(span);
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

