#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include <map>
#include <SDL_mixer.h>

class Sound
{
public:
    Sound();
    ~Sound();

    void playwav(const std::string& name);

private:
    typedef std::multimap<std::string,Mix_Chunk*> chunks_t;
    chunks_t waves;
    bool audioOpen;
    std::string getIdName(const std::string filename);
};

Sound* getSound();

#endif
