#ifndef __DOCUMENTELEMENT_HPP__
#define __DOCUMENTELEMENT_HPP__

class Style;
class XmlReader;

/** Interface for a component that can be part of a text component. This
 * interface gives access to the defined formatting style.
 */
class DocumentElement
{
public:
    virtual ~DocumentElement()
    {}

    virtual const Style& getStyle() const = 0;
    virtual void parse(XmlReader& reader, const Style& parentstyle) = 0;
};

#endif

