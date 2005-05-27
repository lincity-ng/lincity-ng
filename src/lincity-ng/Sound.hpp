/*
Copyright (C) 2005 Wolfgang Becker <uafr@gmx.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
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
