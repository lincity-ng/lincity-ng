#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include "gui/Component.hpp"
#include "gui/XmlReader.hpp"
#include <SDL_mixer.h>


class Sound : public Component
{
    public:
        Sound();
        ~Sound();

        void parse(XmlReader& reader);
        void playwav( int id );
    private:
        static const int maxWaves = 15;
        Mix_Chunk* waves[ maxWaves ];
        bool audioOpen;
};

Sound* getSound();

#endif
