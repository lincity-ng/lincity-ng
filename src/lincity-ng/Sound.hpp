#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include "gui/Component.hpp"
#include "gui/XmlReader.hpp"
#include <map>
#include <SDL_mixer.h>

typedef std::multimap<std::string,Mix_Chunk*> chunks_t;

class Sound : public Component
{
    public:
        Sound();
        ~Sound();

        void parse(XmlReader& reader);
        void playwav( const std::string name );
    private:
        chunks_t waves;
        bool audioOpen;
        std::string getIdName(const std::string filename);
};

Sound* getSound();

#endif
