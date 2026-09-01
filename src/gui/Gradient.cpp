/* ---------------------------------------------------------------------- *
 * src/gui/Gradient.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
 * Copyright (C) 2026      Marc Young <myoung008@gmail.com>
 * Copyright (C) 2026      David Bears <dbear4q@gmail.com>
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

#include <SDL3/SDL.h>                     // for SDL_Surface, SDL_CreateRGBS...
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <stdexcept>                      // for runtime_error
#include <string>                         // for basic_string, operator==
#include <cmath>
#include <cstdint>

#include "ComponentFactory.hpp"           // for IMPLEMENT_COMPONENT_FACTORY
#include "Gradient.hpp"
#include "Painter.hpp"                    // for Painter
#include "Texture.hpp"                    // for Texture
#include "TextureManager.hpp"             // for TextureManager, texture_man...
#include "Vector2.hpp"                    // for Vector2
#include "util/xmlutil.hpp"               // for unexpectedXmlAttribute

Gradient::Gradient()
  : angle(0.f)
{ }

Gradient::~Gradient()
{}

void
Gradient::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(name == "from")
      from.parse(value);
    else if(name == "to")
      to.parse(value);
    else if(name == "angle")
      angle = xmlParse<float>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  flags |= FLAG_RESIZABLE;
}

void
Gradient::resize(float width, float height) {
  if(width < 0) width = 0;
  if(height < 0) height = 0;

  const Vector2 scale = getScale();
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  constexpr SDL_PixelFormat pixfmt = SDL_PIXELFORMAT_RGBA8888;
#else
  constexpr SDL_PixelFormat pixfmt = SDL_PIXELFORMAT_ABGR8888;
#endif
  const SDL_PixelFormatDetails *fmtDetail = SDL_GetPixelFormatDetails(pixfmt);
  SDL_Surface* surface = SDL_CreateSurface(
    (int)std::roundf(width * scale.x),
    (int)std::roundf(height * scale.y),
    pixfmt
  );
  if(!surface)
    throw std::runtime_error("Couldn't create SDL_Surface for gradient.");
  if(!surface->w || !surface->h) {
    SDL_DestroySurface(surface);
    texture.reset();
    this->width = width;
    this->height = height;
    return;
  }


  int lenx = surface->w - 1; // `to` color lands in the center of the last pixel
  int leny = surface->h - 1;
  float cos = std::cosf(angle);
  float sin = std::sinf(angle);
  float dist = lenx * cos + leny * sin; // length of the gradient
  float dx = dist ? cos / dist : 0.f;
  float dy = dist ? sin / dist : 0.f;
  float drdx = (to.r - from.r) * dx;
  float dgdx = (to.g - from.g) * dx;
  float dbdx = (to.b - from.b) * dx;
  float dadx = (to.a - from.a) * dx;
  float drdy = (to.r - from.r) * dy;
  float dgdy = (to.g - from.g) * dy;
  float dbdy = (to.b - from.b) * dy;
  float dady = (to.a - from.a) * dy;
  float originr = (-lenx * drdx - leny * drdy + to.r + from.r) / 2.f;
  float origing = (-lenx * dgdx - leny * dgdy + to.g + from.g) / 2.f;
  float originb = (-lenx * dbdx - leny * dbdy + to.b + from.b) / 2.f;
  float origina = (-lenx * dadx - leny * dady + to.a + from.a) / 2.f;

  for(int j = 0; j < surface->h; j++) {
    float originrx = originr + j * drdy;
    float origingx = origing + j * dgdy;
    float originbx = originb + j * dbdy;
    float originax = origina + j * dady;
    uint32_t *row =
      (uint32_t *)((uint8_t *)surface->pixels + j * surface->pitch);
    for(int i = 0; i < surface->w; i++) {
      row[i] =
        (uint32_t)std::roundf(originrx + i * drdx) << fmtDetail->Rshift |
        (uint32_t)std::roundf(origingx + i * dgdx) << fmtDetail->Gshift |
        (uint32_t)std::roundf(originbx + i * dbdx) << fmtDetail->Bshift |
        (uint32_t)std::roundf(originax + i * dadx) << fmtDetail->Ashift;
    }
  }

  texture.reset(texture_manager->create(surface));
  SDL_DestroySurface(surface);
  this->width = width;
  this->height = height;
}

void
Gradient::draw(Painter& painter) {
  if(texture)
    painter.drawTexture(texture.get(), Vector2(0, 0));
}

IMPLEMENT_COMPONENT_FACTORY(Gradient)

/** @file gui/Gradient.cpp */
