#ifndef __DOCUMENT_HPP__
#define __DOCUMENT_HPP__

#include <vector>
#include "Style.hpp"
#include "Component.hpp"
#include "callback/Signal.hpp"

class XmlReader;
class Paragraph;

class Document : public Component
{
public:
    Document();
    virtual ~Document();

    void parse(XmlReader& reader);

    void draw(Painter& painter);
    void resize(float width, float height);

    Style style;
    Signal<Paragraph*, const std::string& > linkClicked;

private:
    void paragraphLinkClicked(Paragraph* paragraph, const std::string& href);
};

#endif

