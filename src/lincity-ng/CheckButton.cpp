#include "CheckButton.hpp"
#include "gui/Event.hpp"
#include "gui/ComponentFactory.hpp"

CheckButton::CheckButton(Component *pParent,XmlReader &reader):
  Button(pParent,reader),checked(false)
{
}
void CheckButton::event(const Event& event)
{
    switch(event.type) {
        case Event::MOUSEMOTION:
            if(event.inside) {
                if(state == STATE_NORMAL) {
                    state = STATE_HOVER;
                }
            } else {
                if(state == STATE_HOVER) {
                    state = STATE_NORMAL;
                }
            }
            if(checked)
              state=STATE_CLICKED;
            break;
        case Event::MOUSEBUTTONDOWN:
            if(event.inside) {
                state = STATE_CLICKED;
            } else {
                state = STATE_NORMAL;
            }
            if(checked)
              state=STATE_CLICKED;
            break;
        case Event::MOUSEBUTTONUP:
            if(event.inside && state == STATE_CLICKED) {
                printf("Clicked on Button '%s'.\n", getName().c_str());
                clicked(this);
                checked=!checked;
            }
            state = event.inside ? STATE_HOVER : STATE_NORMAL;
            if(checked)
              state=STATE_CLICKED;
            break;
        default:
            break;
    }

    Component::event(event);
}

IMPLEMENT_COMPONENT_FACTORY(CheckButton)
