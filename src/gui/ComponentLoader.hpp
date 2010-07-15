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
 * @file ComponentLoader.hpp
 */

#ifndef __COMPONENTLOADER_HPP__
#define __COMPONENTLOADER_HPP__

#include <string>

class Component;
class XmlReader;

/**
 * Loads a gui-definition xml-file from disk and returns the toplevel
 * component
 */
Component* loadGUIFile(const std::string& filename);

/**
 * Parse a component embedded in the current xml element
 * ie. In this xml-tree: 
 * <Contents>
 *   <Image src="example.png"/>
 * </Contents>
 * the Image component is embedded in the Contents element.
 */
Component* parseEmbeddedComponent(XmlReader& reader);

/**
 * Given the name of the component and an xml-reader, this function will parse
 * the component and return it.
 */
Component* createComponent(const std::string& type, XmlReader& reader);

#endif

/** @file gui/ComponentLoader.hpp */

