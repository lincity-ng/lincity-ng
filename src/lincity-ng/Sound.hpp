#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include <map>
#include <SDL_mixer.h>
#include <SDL.h> 
#include <SDL_thread.h>

class Sound
{
public:
    Sound();
    ~Sound();

    void playwav(const std::string& name);
private:
    static int soundThread( void* ptr );
    typedef std::multimap<std::string,Mix_Chunk*> chunks_t;
    chunks_t waves;
    bool audioOpen;
    std::string getIdName(const std::string filename);
    SDL_Thread* loaderThread;
    void loadWaves();
};

Sound* getSound();

#endif
