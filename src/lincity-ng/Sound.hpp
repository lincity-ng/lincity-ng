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
#include <vector>
#include <SDL_mixer.h>
#include <SDL.h> 
#include <SDL_thread.h>

//TODO make use for these:
enum MusicState {
    PLAY, PAUSE, STOP  
};

//Used for changing tracks.
enum MusicTransport {
    NEXT_TRACK, PREV_TRACK, NEXT_OR_FIRST_TRACK
};

struct song {
    std::string title;
    std::string filename;
    int trackNumber;
    float lowestTechLevel;
    float highestTechLevel;
};

class Sound
{
public:
    Sound();
    ~Sound();

    void playSound(const std::string& name);
    void playASound(Mix_Chunk *chunk);
    void playMusic();
    void changeTrack(MusicTransport command);
    void enableMusic(bool enabled);
    /** set Music volume 0..100, 0=silent */
    void setMusicVolume(int vol);
    /** set Sound effect volume (0..100) */
    void setSoundVolume(int vol);
    /** Load song data from theme directory */
    void loadMusicTheme();
    /** information about currently playing track. Title, filename, track number, and tech levels allowed to play it. */
    song currentTrack;


private:
    static int soundThread(void* ptr);
    void loadWaves();
    std::string getIdName(const std::string& filename);

    typedef std::multimap<std::string,Mix_Chunk*> chunks_t;
    chunks_t waves;
    bool audioOpen;
    SDL_Thread* loaderThread;
    Mix_Music* currentMusic;
    int totalTracks;
    std::vector<song> playlist;
};

Sound* getSound();

#endif

/** @file lincity-ng/Sound.hpp */

