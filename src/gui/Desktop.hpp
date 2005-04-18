#ifndef __DESKTOP_HPP__
#define __DESKTOP_HPP__

#include <vector>
#include "Component.hpp"

class XmlReader;

class Desktop : public Component
{
public:
    Desktop();
    virtual ~Desktop();

    void parse(XmlReader& reader);

    void resize(float width, float height);
    void event(const Event& event);
    void draw(Painter& painter);
    bool opaque(const Vector2& pos) const;

    void addChildComponent(Component* child);

    Vector2 getPos(Component* component);
    void move(Component* component, Vector2 pos);
    void remove(Component* component);

protected:
    void setDirty(const Rect2D& rect);
    
private:
    void internal_remove(Component* component);
    
    std::vector<Component*> removeQueue;
    std::vector<Component*> addQueue;
    typedef std::vector<Rect2D> DirtyRectangles;
    DirtyRectangles dirtyRectangles;
};

#endif

