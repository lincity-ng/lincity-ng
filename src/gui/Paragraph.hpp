#ifndef __TEXTBOX_HPP__
#define __TEXTBOX_HPP__

#include <vector>
#include <string>
#include "Texture.hpp"
#include "Component.hpp"
#include "Style.hpp"

class XmlReader;

/** contains a part of text in normalized form (ie. all spaces collapsed to ' ')
 */
class TextSpan
{
public:
    Style style;
    std::string text;
};

/**
 * A paragraph of text
 */
class Paragraph : public Component
{
public:
    Paragraph(Component* parent, XmlReader& reader);
    Paragraph(Component* parent, XmlReader& reader, Style parentstyle);
    virtual ~Paragraph();

    virtual void resize(float width, float height);
    virtual void draw(Painter& painter);
    virtual void event(Event& event);

    typedef std::vector<TextSpan*> TextSpans;
    TextSpans textspans;
    Style style;

private:
    void parse(XmlReader& reader, Style parentstyle);
    
    Texture* texture;
};

#endif

