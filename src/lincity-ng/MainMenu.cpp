#include <config.h>

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <typeinfo>
#include <physfs.h>

#include "gui/TextureManager.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Component.hpp"
#include "gui/Event.hpp"
#include "gui/Button.hpp"
#include "gui/callback/Callback.hpp"

#include "lincity/lin-city.h"

#include "CheckButton.hpp"

#include "MainMenu.hpp"
#include "Util.hpp"

#include "Sound.hpp"

MainMenu::MainMenu()
{
    loadMainMenu();
    currentMenu = mainMenu.get();
}

MainMenu::~MainMenu()
{
}

void
MainMenu::loadMainMenu()
{
    if(mainMenu.get() == 0) {
        mainMenu.reset(loadGUIFile("gui/mainmenu.xml"));

        // connect signals
        Button* quitButton = getButton(*mainMenu, "QuitButton");
        quitButton->clicked.connect(
                makeCallback(*this, &MainMenu::quitButtonClicked));
        Button* continueButton = getButton(*mainMenu, "ContinueButton"); 
        continueButton->clicked.connect(
                makeCallback(*this, &MainMenu::continueButtonClicked));
        Button* newGameButton = getButton(*mainMenu, "NewGameButton");
        newGameButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameButtonClicked));
        Button* loadGameButton = getButton(*mainMenu, "LoadButton");
        loadGameButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameButtonClicked));
    }

    mainMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void MainMenu::fillNewGameMenu()
{
  char *buttonNames[]={"File0","File1","File2","File3","File4","File5"};
  
  char **files= PHYSFS_enumerateFiles("data/opening");
  
  char **fptr=files;
 
  for(int i=0;i<6;i++)
  {
    CheckButton *button=getCheckButton(*newGameMenu.get(),buttonNames[i]);
    
    button->clicked.connect(makeCallback(*this,&MainMenu::selectLoadGameButtonClicked));
    while(*fptr)
    {
      if(std::string(*fptr).find(".scn")!=std::string::npos)
        break;
      fptr++;
    }
    if(*fptr)
    {
      std::string f=*fptr;
      if(f.length()>5)
        f=f.substr(0,f.length()-4); // truncate .scn
      button->setCaptionText(f);
      fptr++;
    }
    else
      button->setCaptionText("");
  }
  PHYSFS_freeList(files);
}
void MainMenu::fillLoadMenu()
{
  char *buttonNames[]={"File0","File1","File2","File3","File4","File5"};
  //char *scenario[]={"Scenario0","Scenario1"};
  
  char **files= PHYSFS_enumerateFiles("data/savegames");
  
  char **fptr=files;
 
  for(int i=0;i<6;i++)
  {
    CheckButton *button=getCheckButton(*loadGameMenu.get(),buttonNames[i]);
    
    button->clicked.connect(makeCallback(*this,&MainMenu::selectLoadGameButtonClicked));
    while(*fptr)
    {
      if(std::string(*fptr).find(".scn")!=std::string::npos)
        break;
      fptr++;
    }
    if(*fptr)
    {
      std::string f=*fptr;
      if(f.length()>5)
        f=f.substr(0,f.length()-4); // truncate .scn
      button->setCaptionText(f);
      fptr++;
    }
    else
      button->setCaptionText("");
  }
  PHYSFS_freeList(files);
}

void
MainMenu::loadNewGameMenu()
{
    if(newGameMenu.get() == 0) {
        newGameMenu.reset(loadGUIFile("gui/newgame.xml"));

        // connect signals
        Button* startButton = getButton(*newGameMenu, "StartButton");
        startButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameStartButtonClicked));
        Button* backButton = getButton(*newGameMenu, "BackButton");
        backButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameBackButtonClicked));
        fillNewGameMenu();
    }

    newGameMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void
MainMenu::loadLoadGameMenu()
{
    if(loadGameMenu.get() == 0) {
        loadGameMenu.reset(loadGUIFile("gui/loadgame.xml"));

        // connect signals
        Button* loadButton = getButton(*loadGameMenu, "LoadButton");
        loadButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameLoadButtonClicked));
        Button* backButton = getButton(*loadGameMenu, "BackButton");
        backButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameBackButtonClicked));
        // fill in file-names into slots
        fillLoadMenu();
    }

    loadGameMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void 
MainMenu::selectLoadGameButtonClicked(CheckButton* button ,int)
{
  std::string fc=button->getCaptionText();
  
  std::string file="";
  if(button->getName()=="Scenario0")
    file="data/opening/good_times.scn";
  else if(button->getName()=="Scenario1")
    file="data/opening/bad_times.scn";
  else if(fc.length())
  {
    if(newGameMenu.get()==currentMenu)
      file=std::string("data/opening/")+fc+".scn";
    else
      file=std::string("data/savegames/")+fc+".scn";
  }
  char *bs[]={"File0","File1","File2","File3","File4","File5",""};
  for(int i=0;std::string(bs[i]).length();i++)
  {
    CheckButton *b=getCheckButton(*currentMenu,bs[i]);
    if(b->getName()!=button->getName())
      b->uncheck();
  }
  mFilename=file;
}

    
void
MainMenu::quitButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    quitState = QUIT;
    running = false;
}

void
MainMenu::continueButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    quitState = INGAME;
    running = false;
}

void
MainMenu::newGameButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadNewGameMenu();
    currentMenu = newGameMenu.get();
}

void
MainMenu::loadGameButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadLoadGameMenu();
    currentMenu = loadGameMenu.get();
}

void
MainMenu::newGameStartButtonClicked(Button* )
{
    if(mFilename.length())
      load_city(const_cast<char*>(mFilename.c_str()));
    getSound()->playSound( "Click" );
    quitState = INGAME;
    running = false;
}

void
MainMenu::newGameBackButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadMainMenu();
    currentMenu = mainMenu.get();
}

void
MainMenu::loadGameBackButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadMainMenu();
    currentMenu = mainMenu.get();
}

void
MainMenu::loadGameLoadButtonClicked(Button *)
{
    if(mFilename.length())
      load_city(const_cast<char*>(mFilename.c_str()));
    getSound()->playSound( "Click" );
    quitState = INGAME;
    running = false;
}

MainState
MainMenu::run()
{
    SDL_Event event;
    running = true;
    quitState = QUIT;
    Uint32 ticks = SDL_GetTicks();
    int frame = 0;
    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_VIDEORESIZE:
                    initVideo(event.resize.w, event.resize.h);
                    currentMenu->resize(event.resize.w, event.resize.h);
                    break;
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    Event gui_event(event);
                    currentMenu->event(gui_event);
                    break;
                }
                case SDL_QUIT:
                    running = false;
                    quitState = QUIT;
                    break;
                default:
                    break;
            }
        }

        currentMenu->draw(*painter);
        flipScreenBuffer();

        frame++;
        if(SDL_GetTicks() - ticks > 1000) {
            printf("FPS: %d.\n", frame);
            frame = 0;
            ticks = SDL_GetTicks();
        }
    }

    return quitState;
}
