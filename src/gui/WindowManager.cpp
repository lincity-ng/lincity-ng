
#include "WindowManager.hpp"

#include <iostream>

#include "ComponentFactory.hpp"   // for IMPLEMENT_COMPONENT_FACTORY
#include "Desktop.hpp"
#include "XmlReader.hpp"

// some macros to reduce code duplication when using Vector2
#define AXIS(P, X, Y) (AXISID?(P Y):(P X))
#define XY(V) AXIS((V).,x,y)
#define EACHAXIS(CODE) {int AXISID=0;CODE AXISID=1;CODE}


WindowManager::WindowManager() {
  setFlags(FLAG_RESIZABLE);
}

WindowManager::~WindowManager() {

}

void
WindowManager::parse(XmlReader& reader) {
  XmlReader::AttributeIterator iter(reader);
  while(iter.next()) {
    const char* attribute = (const char*) iter.getName();
    const char* value = (const char*) iter.getValue();

    if(parseAttribute(attribute, value)) {
      continue;
    } else {
      std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
    }
  }

  int depth = reader.getDepth();
  while(reader.read() && reader.getDepth() > depth) {
    if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
      std::string element = (const char*) reader.getName();
      std::cerr << "Skipping unknown element '" << element << "'.\n";
      reader.nextNode();
    }
  }
}

void
WindowManager::resize(float width, float height) {
  size = Vector2(width, height);
  for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
    Child &child = *i;
    Window *window = static_cast<Window *>(child.getComponent());
    Vector2 childPos = child.getPos();
    child.setPos(childPos.constrain(Rect2D(Vector2(), size - window->size)));
    window->size.constrain(Rect2D(Vector2(), size));
    window->reLayout();
  }
}

void
WindowManager::event(const Event& event) {
  lockChilds();
  bool visible = event.inside;

  if(event.type == Event::MOUSEBUTTONDOWN
    && event.mousebutton != SDL_BUTTON_LEFT
  ) {
    // cancel window resize because a wrong mouse button is pressed
    std::cout << "canceling drag" << '\n';
    dragWindow = NULL;
    dragEdge = Edge::NONE;
    desktop->tryClearCursor(this);
  }
  else if(event.type == Event::MOUSEBUTTONUP
    && dragEdge == Edge::NSWE && hasMoved
  ) {
    std::cout << "ending move" << '\n';
    desktop->tryClearCursor(this);
    hasMoved = false;
    visible = false;
  }

  for(Childs::reverse_iterator i = childs.rbegin(); i != childs.rend(); ++i) {
    Child &child = *i;
    Window *window = static_cast<Window *>(child.getComponent());
    Vector2 mousepos = event.mousepos - child.getPos();

    if(!child.isEnabled())
      continue;

    switch(event.type) {
    case Event::MOUSEBUTTONDOWN:
      if(dragWindow == window) {
        std::cout << "starting drag" << '\n';
        dragOffset = -mousepos;
        if(!(dragEdge & Edge::W)) {
          dragOffset.x += window->getSize().x;
        }
        if(!(dragEdge & Edge::N)) {
          dragOffset.y += window->getSize().y;
        }
        hasMoved = false;
      }

      if(child.inside(event.mousepos) || dragWindow == window) {
        // TODO: move window to front
      }
      break;

    case Event::MOUSEMOTION: {
      if(event.mousebuttonstate) {
        if(dragEdge == Edge::NSWE) {
          // move the window
          std::cout << "executing move" << '\n';
          Vector2 pos = event.mousepos + dragOffset;
          pos.constrain(Rect2D(Vector2(), getSize() - window->getSize()));
          child.setPos(pos);
          desktop->setSystemCursor(this, SDL_SYSTEM_CURSOR_SIZEALL);
          setDirty();
          hasMoved = true;
          visible = false;
        }
        else if(dragEdge) {
          // resize the window
          std::cout << "executing resize" << '\n';
          const Vector2 oldSize = window->getSize();
          Vector2 newSize = oldSize;
          const Vector2 oldPos = child.getPos();
          Vector2 newPos = event.mousepos + dragOffset;
          newPos.constrain(Rect2D(Vector2(), getSize()));

          EACHAXIS({
            if(dragEdge & AXIS(Edge::,W,N))
              XY(newSize) = XY(oldPos + oldSize - newPos);
            else if(dragEdge & AXIS(Edge::,E,S))
              XY(newSize) = XY(newPos - oldPos);
          })
          if(newSize == oldSize)
            break;

          window->resize(newSize.x, newSize.y);

          EACHAXIS({
            if(!(dragEdge & AXIS(Edge::,W,N)))
              XY(newPos) = XY(oldPos);
          })
          if(newPos != oldPos)
            child.setPos(newPos);
          setDirty();
        }
      }
      else {
        std::cout << "evaluating new edge" << '\n';
        Edge edge = visible ? edgeAt(child, event.mousepos) : Edge::NONE;
        if(!edge && window == dragWindow) {
          std::cout << "mouse exitted drag area" << '\n';
          dragWindow = NULL;
          dragEdge = Edge::NONE;
          desktop->tryClearCursor(this);
        }
        else if(edge && (edge != dragEdge || window != dragWindow)) {
          std::cout << "mouse entered drag area" << '\n';
          dragWindow = window;
          dragEdge = edge;
          int cursorId = SDL_SYSTEM_CURSOR_SIZENWSE;
          switch(dragEdge) {
          case Edge::NSWE:
            cursorId++;
          case Edge::N:
          case Edge::S:
            cursorId++;
          case Edge::W:
          case Edge::E:
            cursorId++;
          case Edge::NE:
          case Edge::SW:
            cursorId++;
          }
          desktop->setSystemCursor(this, (SDL_SystemCursor)cursorId);
        }
      }
    }
      break;

    default:
      break;
    }

    if(window == dragWindow && dragEdge != Edge::NSWE)
      visible = false;

    if(eventChild(child, event, visible))
      visible = false;
  }
  unlockChilds();
}

bool
WindowManager::opaque(const Vector2& pos) const {
  for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
    const Child& child = *i;
    if(!child.getComponent() || !child.isEnabled())
      continue;

    if(child.getComponent()->opaque(pos - child.getPos()) || edgeAt(child, pos))
      return true;
  }

  return false;
}

void
WindowManager::addWindow(Window *window) {
  if(childLock)
    addQueue.push_back(window);
  else
    addWindowInternal(window);
}

void
WindowManager::removeWindow(Window *window) {
  if(childLock)
    removeQueue.push_back(window);
  else
    removeWindowInternal(window);
}

WindowManager::Edge
WindowManager::edgeAt(const Child &child, Vector2 pos) const {
  Window *window = static_cast<Window *>(child.getComponent());
  const Vector2 size = window->getSize();
  pos -= child.getPos();

  int edge = Edge::NONE;
  EACHAXIS({
    if(XY(pos) >= -grabDist && XY(pos) < grabDist)
      edge |= AXIS(Edge::,W,N);
    else if(XY(size) - XY(pos) >= -grabDist && XY(size) - XY(pos) < grabDist)
      edge |= AXIS(Edge::,E,S);
  })

  if(!edge && window->title().inside(pos))
    edge = Edge::NSWE;

  return (Edge)edge;
}

void
WindowManager::addWindowInternal(Window *window) {
  Child& child = addChild(window);
  child.setPos((getSize() - window->getSize()) / 2);
}

void
WindowManager::removeWindowInternal(Window *window) {
  // find child
  for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
    if(i->getComponent() == window) {
      childs.erase(i);
      return;
    }
  }
  throw std::runtime_error(
    "Trying to remove a component that is not a direct child");
}

void
WindowManager::lockChilds() {
  childLock = true;
}

void
WindowManager::unlockChilds() {
  childLock = false;

  // process pending remove events...
  using queue_it = std::vector<Window *>::iterator;
  for(queue_it i = removeQueue.begin(); i != removeQueue.end(); ++i)
    removeWindowInternal(*i);
  removeQueue.clear();

  // process pending child adds...
  for(queue_it i = addQueue.begin(); i != addQueue.end(); ++i )
    addWindowInternal(*i);
  addQueue.clear();
}

IMPLEMENT_COMPONENT_FACTORY(WindowManager)
