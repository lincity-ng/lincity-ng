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
