#ifndef __TEXTBOX_HPP__
#define __TEXTBOX_HPP__

#include <vector>
#include <string>
#include "Texture.hpp"
#include "Component.hpp"
#include "Style.hpp"

class XmlReader;

/**
 * contains a part of text in normalized form (all spaces collapsed to ' ')
 */
class TextSpan
{
public:
    Style style;
    std::string text;
};

/**
 * A paragraph of (formatted) text
 * The text is constructed from a list of TextSpans. A TextSpan combines a list
 * of words with a Style so that it can have different font attributes and
 * color.
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

    /**
     * sets a new text in the Paragraph. The style of the paragraph is used
     */
    void setText(const std::string& text);
    /**
     * Sets a new text in the Paragraph.
     */
    void setText(const std::string& text, const Style& style);

private:
    typedef std::vector<TextSpan*> TextSpans;
    TextSpans textspans;
    Style style;

    void parse(XmlReader& reader, Style parentstyle);
    
    Texture* texture;
};

#endif

