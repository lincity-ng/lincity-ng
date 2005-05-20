#ifndef __TOOLTIPMANAGER_HPP__
#define __TOOLTIPMANAGER_HPP__

#include <map>
#include "Component.hpp"

class XmlReader;
class Paragraph;

class TooltipManager : public Component
{
public:
    TooltipManager();
    ~TooltipManager();

    void parse(XmlReader& reader);

    void resize(float width, float height);
    void event(const Event& event);
    bool opaque(const Vector2& pos) const;

    void showTooltip(const std::string& text, const Vector2& pos);
    
private:
    Child& comp_tooltip()
    {
        return childs[0];
    }
};

/// global TooltipManager instance
extern TooltipManager* tooltipManager;

#endif

