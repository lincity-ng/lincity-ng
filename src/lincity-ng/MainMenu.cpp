#include <config.h>

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <physfs.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>

#include "gui/TextureManager.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Component.hpp"
#include "gui/Event.hpp"
#include "gui/Desktop.hpp"
#include "gui/Button.hpp"
#include "gui/callback/Callback.hpp"

#include "gui_interface/shared_globals.h"
#include "lincity/lin-city.h"
#include "lincity/fileutil.h"

#include "CheckButton.hpp"

#include "MainMenu.hpp"
#include "Util.hpp"
#include "Config.hpp"
#include "Sound.hpp"
#include "GameView.hpp"
#include "MainLincity.hpp"

MainMenu::MainMenu()
{
    loadMainMenu();
    switchMenu(mainMenu.get());
    baseName = "";
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
        Button* creditsButton = getButton(*mainMenu, "CreditsButton");
        creditsButton->clicked.connect(
                makeCallback(*this, &MainMenu::creditsButtonClicked));
        Button* optionsButton = getButton(*mainMenu, "OptionsButton");
        optionsButton->clicked.connect(
                makeCallback(*this, &MainMenu::optionsButtonClicked));
 
    }

    mainMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void MainMenu::fillNewGameMenu()
{
  char *buttonNames[]={"File0","File1","File2","File3","File4","File5"};
  
  char **files= PHYSFS_enumerateFiles("opening");
  
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
  //read savegames from ~/.lincity so we can use the original save_city()
  std::string lincityDirName = PHYSFS_getUserDir();
  lincityDirName+="/.lincity"; 
  DIR* lincityDir = opendir( lincityDirName.c_str() );
  if(!lincityDir) {
#ifdef DEBUG
      std::cerr << "Warning directory " << lincityDirName << " doesn't exist.\n";
#endif
      return;
  }

  dirent* curfile;
  for(int i=0;i<6;i++)
  {
    std::stringstream filestart;
    filestart << i+1 << "_";
    CheckButton *button=getCheckButton(*loadGameMenu.get(),buttonNames[i]);
    
    button->clicked.connect(makeCallback(*this,&MainMenu::selectLoadGameButtonClicked));
    while( ( curfile = readdir( lincityDir ) ) )
    { 
      if(std::string( curfile->d_name ).find( filestart.str() ) == 0 && !( curfile->d_type & DT_DIR  ) )
        break;
    }
    if( curfile )
    {
      std::string f= curfile->d_name;
      button->setCaptionText(f);
    }
    else
      button->setCaptionText("");

    rewinddir( lincityDir );
  }
  closedir( lincityDir );
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
        Button* startBareButton = getButton(*newGameMenu, "StartBareButton");
        startBareButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameStartBareButtonClicked));
        Button* startVillageButton = getButton(*newGameMenu, "StartVillageButton");
        startVillageButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameStartVillageClicked));
        fillNewGameMenu();
    }

    newGameMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void
MainMenu::loadCreditsMenu()
{
    if(creditsMenu.get() == 0) {
        creditsMenu.reset(loadGUIFile("gui/credits.xml"));
        Button* backButton = getButton(*creditsMenu, "BackButton");
        backButton->clicked.connect(
                makeCallback(*this, &MainMenu::creditsBackButtonClicked));
    }

    creditsMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void
MainMenu::loadOptionsMenu()
{
    if(optionsMenu.get() == 0) {
        optionsMenu.reset(loadGUIFile("gui/options.xml"));
        CheckButton* currentCheckButton = getCheckButton(*optionsMenu, "BackgroundMusic");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "SoundFX");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "Fullscreen");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        
        Button* currentButton = getButton(*optionsMenu, "BackButton");
        currentButton->clicked.connect( makeCallback(*this, &MainMenu::creditsBackButtonClicked));
    }
    //adjust checkbutton-states
    if( getConfig()->musicEnabled ){
        getCheckButton(*optionsMenu, "BackgroundMusic")->check();
    } else {
        getCheckButton(*optionsMenu, "BackgroundMusic")->uncheck();
    }
    if( getConfig()->soundEnabled ){
        getCheckButton(*optionsMenu, "SoundFX")->check();
    } else {
        getCheckButton(*optionsMenu, "SoundFX")->uncheck();
    }
    if( getConfig()->useFullScreen ){
        getCheckButton(*optionsMenu, "Fullscreen")->check();
    } else {
        getCheckButton(*optionsMenu, "Fullscreen")->uncheck();
    }
    optionsMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
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
        Button* saveButton = getButton(*loadGameMenu, "SaveButton");
        saveButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameSaveButtonClicked));
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
    file="opening/good_times.scn";
  else if(button->getName()=="Scenario1")
    file="opening/bad_times.scn";
  else if(fc.length())
  {
    baseName = fc;
    if(newGameMenu.get()==currentMenu )
      file=std::string("opening/")+fc+".scn";
    else{
      file=fc;
    }
  }
  char *bs[]={"File0","File1","File2","File3","File4","File5",""};
  for(int i=0;std::string(bs[i]).length();i++)
  {
    CheckButton *b=getCheckButton(*currentMenu,bs[i]);
    if(b->getName()!=button->getName())
      b->uncheck();
  }

  if(newGameMenu.get()==currentMenu){
    mFilename=PHYSFS_getRealDir( file.c_str() );
  } else {
    slotNr = 1 + atoi( const_cast<char*>(button->getName().substr(4).c_str()) );
    if( file.length() == 0){
        mFilename = "";
        return;
    }
    mFilename=PHYSFS_getUserDir();
    mFilename+="/.lincity"; 
  }
    mFilename+="/";
    mFilename+=file;
}

void MainMenu::optionsMenuButtonClicked( CheckButton* button, int ){
    std::string buttonName = button->getName();
    if( buttonName == "BackgroundMusic"){
        getSound()->playSound("Click");
        getSound()->enableMusic( !getConfig()->musicEnabled );
    } else if( buttonName == "SoundFX"){
        getConfig()->soundEnabled = !getConfig()->soundEnabled;
        getSound()->playSound("Click");
    } else if( buttonName == "Fullscreen"){
        getSound()->playSound("Click");
        getConfig()->useFullScreen = !getConfig()->useFullScreen; 
        initVideo(getConfig()->videoX, getConfig()->videoY);
        loadOptionsMenu();
    } else {
        std::cerr << "MainMenu::optionsMenuButtonClicked " << buttonName << " unknown Button!\n";
    }    
}

void
MainMenu::quitButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    quitState = QUIT;
    running = false;
}

void
MainMenu::switchMenu(Component* newMenu)
{
    currentMenu = dynamic_cast<Desktop*> (newMenu);
    if(!currentMenu)
        throw std::runtime_error("Menu Component is not a Desktop");
}

void
MainMenu::creditsButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadCreditsMenu();
    switchMenu(creditsMenu.get());
}

void
MainMenu::optionsButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadOptionsMenu();
    switchMenu(optionsMenu.get());
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
    switchMenu(newGameMenu.get());
}

void
MainMenu::loadGameButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadLoadGameMenu();
    switchMenu(loadGameMenu.get());
}

void
MainMenu::creditsBackButtonClicked(Button* )
{
    getSound()->playSound("Click");
    loadMainMenu();
    switchMenu(mainMenu.get());
}

void
MainMenu::newGameStartButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    if( loadCityNG( mFilename ) ){
    	strcpy (given_scene, baseName.c_str());
        quitState = INGAME;
        running = false;
    }
}

void
MainMenu::newGameStartBareButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    new_city( &main_screen_originx, &main_screen_originy, 0 );
    given_scene[0] = 0;
    for( int i = 0; i < monthgraph_size; i++ ){
        monthgraph_pop[i] = 0;
        monthgraph_starve[i] = 0;
        monthgraph_nojobs[i] = 0;
        monthgraph_ppool[i] = 0;
    } 
    GameView* gv = getGameView();
    if( gv ){ gv->readOrigin(); }
    quitState = INGAME;
    running = false;
}

void
MainMenu::newGameStartVillageClicked(Button* )
{
    getSound()->playSound( "Click" );
    new_city( &main_screen_originx, &main_screen_originy, 1 );
    given_scene[0] = 0;
    for( int i = 0; i < monthgraph_size; i++ ){
        monthgraph_pop[i] = 0;
        monthgraph_starve[i] = 0;
        monthgraph_nojobs[i] = 0;
        monthgraph_ppool[i] = 0;
    } 
    GameView* gv = getGameView();
    if( gv ){ gv->readOrigin(); }
    quitState = INGAME;
    running = false;
}

void
MainMenu::newGameBackButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadMainMenu();
    switchMenu(mainMenu.get());
}

void
MainMenu::loadGameBackButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadMainMenu();
    switchMenu(mainMenu.get());   
}

void
MainMenu::gotoMainMenu()
{
    getSound()->playSound( "Click" );
    loadMainMenu();
    switchMenu(mainMenu.get());   
}

void
MainMenu::loadGameLoadButtonClicked(Button *)
{
    getSound()->playSound( "Click" );
    if( loadCityNG( mFilename ) ){
        quitState = INGAME;
        running = false;
    }
}

void
MainMenu::loadGameSaveButtonClicked(Button *)
{
    time_t now = time(NULL);
    struct tm* datetime = localtime(&now);

    getSound()->playSound( "Click" );
    if( file_exists( const_cast<char*>(mFilename.c_str()) ) ){
        std::cout << "remove( " << mFilename << ")\n";
        remove( mFilename.c_str() );
    }
    std::stringstream newStart;
    newStart << slotNr << "_" << (1900 + datetime->tm_year) << "-";
    newStart << std::setfill('0') << std::setw(2);
    newStart << datetime->tm_mon << "-" << datetime->tm_mday << "_";
    newStart << datetime->tm_hour << ":" << datetime->tm_min;
    std::string newFilename( newStart.str() ); 
    saveCityNG( newFilename );
    fillLoadMenu();
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

        if(currentMenu->needsRedraw()) {
            currentMenu->draw(*painter);
            flipScreenBuffer();
        } else {
            // give the CPU time to relax...
            SDL_Delay(25);
        }

        frame++;
        if(SDL_GetTicks() - ticks > 1000) {
            printf("FPS: %d.\n", frame);
            frame = 0;
            ticks = SDL_GetTicks();
        }
    }

    return quitState;
}
