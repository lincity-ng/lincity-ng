#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include <map>
#include <string>
#include <SDL_mixer.h>
#include <SDL.h> 
#include <SDL_thread.h>

class Sound
{
public:
    Sound();
    ~Sound();

    void playSound(const std::string& name);
    void playMusic(const std::string& name);
    void enableMusic(bool enabled);
    /** set Music volume 0..100, 0=silent*/
    void setMusicVolume(int vol);
    /** set Sound effect volume (0..100) */
    void setSoundVolume(int vol);

private:
    static int soundThread(void* ptr);
    void loadWaves();
    std::string getIdName(const std::string& filename);

    typedef std::multimap<std::string,Mix_Chunk*> chunks_t;
    chunks_t waves;
    bool audioOpen;
    SDL_Thread* loaderThread;
    Mix_Music* currentMusic;
    std::string musicFile;
};

Sound* getSound();

#endif
