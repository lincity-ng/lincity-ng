#include "Util.hpp"

#include <stdexcept>
#include <sstream>

#include "gui/Button.hpp"
#include "gui/Component.hpp"
#include "gui/Paragraph.hpp"

template<typename T>
void findComponent(T** result, Component& tree, const std::string& name)
{
    Component* component = tree.findComponent(name);
    if(!component) {
        std::stringstream msg;
        msg << "GUI file didn't define '" << name << "' (type "
            << typeid(T).name() << ")";
        throw std::runtime_error(msg.str());
    }
    T* casted_component = dynamic_cast<T*> (component);
    if(!casted_component) {
        std::stringstream msg;
        msg << "Component '" << name << "' is of type "
            << typeid(*component).name() << " but "
            << typeid(T).name() << " is expected.";
        throw std::runtime_error(msg.str());
    }
    *result = casted_component;
}

Button* getButton(Component& tree, const std::string& name)
{
    Button* result;
    findComponent(&result, tree, name);
    return result;
}

Paragraph* getParagraph(Component& tree, const std::string& name)
{
    Paragraph* result;
    findComponent(&result, tree, name);
    return result;
}

