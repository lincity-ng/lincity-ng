#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "main.hpp"
#include <memory>

class Painter;
class Sound;
class Component;

class Game
{
public:
    Game();
    ~Game();

    MainState run();

private:
    std::auto_ptr<Component> gui;
    std::auto_ptr<Sound> sound;
    
    bool running;
    MainState quitState;
};

#endif

