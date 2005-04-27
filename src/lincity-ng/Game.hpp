#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "main.hpp"
#include <memory>
#include "gui/Button.hpp"

class Painter;
class Sound;
class Component;

class Game
{
public:
    Game();
    ~Game();

    MainState run();
    void gameButtonClicked( Button* button );
    void openHelpWindow();

private:
    std::auto_ptr<Component> gui;
    
    bool running;
    MainState quitState;
    void backToMainMenu();
};

#endif

