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
 * @file ComponentFactory.hpp
 */

#ifndef __COMPONENT_FACTORY_HPP__
#define __COMPONENT_FACTORY_HPP__

#include <map>
#include <string>
#include <memory>

const char * GUI_TRANSLATE(const char * msgid);
std::string  GUI_TRANSLATE(const std::string& msgid);

class Component;
class XmlReader;

/**
 * @class Factory
 */
class Factory
{
public:
    virtual ~Factory()
    { }

    virtual Component* createComponent(XmlReader& reader) = 0;
};

typedef std::map<std::string, Factory*> ComponentFactories;
extern ComponentFactories* component_factories;

/**
 * @note From Matze:
 * Yes I know macros are evil, but in this specific case they save
 * A LOT of typing and evil code duplication.
 * I'll happily accept alternatives if someone can present me one that does
 * not involve typing 4 or more lines for each object class
 */
#define DECLARE_COMPONENT_FACTORY(CLASS)                                    \
class INTERN_##CLASS##Factory : public Factory                              \
{                                                                           \
public:                                                                     \
  INTERN_##CLASS##Factory()                                                 \
  {                                                                         \
    if(component_factories == 0)                                            \
      component_factories = new ComponentFactories;                         \
                                                                            \
    component_factories->insert(std::make_pair(#CLASS, this));              \
  }                                                                         \
                                                                            \
  virtual Component* createComponent(XmlReader& reader)                     \
  {                                                                         \
      std::auto_ptr<CLASS> component (new CLASS());                         \
      component->parse(reader);                                             \
      return component.release();                                           \
  }                                                                         \
};
#define IMPLEMENT_COMPONENT_FACTORY(CLASS)                                  \
DECLARE_COMPONENT_FACTORY(CLASS)                                            \
INTERN_##CLASS##Factory factory_##CLASS;

#endif


/** @file gui/ComponentFactory.hpp */
/*      CLASS * component = dynamic_cast<CLASS *>(new CLASS());

      */
