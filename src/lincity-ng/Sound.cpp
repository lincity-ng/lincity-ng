/**
 *  Sound Component for Lincity-NG.
 *   
 *  February 2005, Wolfgang Becker <uafr@gmx.de>
 *
 *  20050224
 *  sound as own class
 *
 *  20050225
 *  read sounds from physfs
 *  
 */
#include <config.h>

#include "Sound.hpp"

#include <assert.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "gui/PhysfsStream/PhysfsSDL.hpp"

#include <SDL_mixer.h>
#include <physfs.h>

Sound* soundPtr = 0;

Sound *getSound()
{
  return soundPtr;
}

Sound::Sound()
{
    assert( soundPtr == 0);
    soundPtr = this;

    //Load Sound
    audioOpen = false;
    /* Open the audio device */
    if (Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    } else {
        audioOpen = true;
                                                                                  
        //Load Waves
        std::string filename;
        std::string directory = "sounds/";
        std::string fullname;
        Mix_Chunk *chunk;
        SDL_RWops* file;
        char **rc = PHYSFS_enumerateFiles( directory.c_str() );
        char **i;
        for (i = rc; *i != NULL; i++) {
            fullname = directory;
            fullname.append( *i );
            filename.assign( *i );

            if(PHYSFS_isDirectory(fullname.c_str()))
                continue;
                
            try {        
                file = getPhysfsSDLRWops( fullname.c_str() );
                chunk = Mix_LoadWAV_RW( file, 1);
                if(!chunk) {
                    std::stringstream msg;
                    msg << "Couldn't read soundfile '" << fullname
                        << "': " << SDL_GetError();
                    throw std::runtime_error(msg.str());
                }
                        
                std::string idName = getIdName( filename );
                waves.insert( std::pair<std::string,Mix_Chunk*>(idName, chunk) );
            } catch(std::exception& e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
        }
        PHYSFS_freeList(rc);
    }
}

Sound::~Sound()
{
    if( soundPtr == this )
    {
        soundPtr = 0;
    }
    for (chunks_t::iterator i = waves.begin(); i != waves.end(); i++) {
        Mix_FreeChunk( i->second );
    }
	if ( audioOpen ) {
		Mix_CloseAudio();
		audioOpen = false;
	}
}

/*
 *  Playback an Audio-Effect.
 *  Name is the Name of an Audiofile from sounds/ minus .wav
 *  and without trailing Numbers. If there are eg.
 *  beep1.wav, beep2.wav, beep3.wav
 *  playwav( "beep" ) would pick one of the three FIles randomly
 */
void Sound::playwav(const std::string& name) {
    std::cout << "Audio " << name << " request...";
    
    if( !audioOpen ){
		std::cout << "Can't play Audio.";
        return;
    }

    chunks_t::size_type count = waves.count( name );
    if ( count == 0 ) {
        std::cout << "No such file \n";
        return;
    }

    chunks_t::iterator it = waves.find(name);
    for (int i = rand() % count; i > 0; i--) {
        it++;
    }
    Mix_PlayChannel( 0, it->second, 0 ); 
    std::cout << "done.\n";
}

/*
 * Get ID-String for a given Filename.
 */
std::string Sound::getIdName( const std::string filename)
{
    std::string::size_type pos = filename.find_first_of(".0123456789");

    return filename.substr(0, pos);
}

