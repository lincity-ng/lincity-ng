/**
 *  Sound Component for Lincity-NG.
 *   
 *  February 2005, Wolfgang Becker <uafr@gmx.de>
 *
 *  20050224
 *  sound as own class
 *
 *  TODO: use MultiMap for storage
 *        use physfs to load data
 *  
 */


#include <config.h>

#include "Sound.hpp"

#include "gui/XmlReader.hpp"
#include "gui/ComponentFactory.hpp"

#include <SDL_mixer.h>

Sound* soundPtr = 0;

Sound *getSound()
{
  return soundPtr;
}

Sound::Sound()
{
    assert( soundPtr == 0);
    soundPtr = this;
}

Sound::~Sound()
{
    if( soundPtr == this )
    {
        soundPtr = 0;
    }
    for ( int i = 0; i < maxWaves; i++ )
        if( waves[i] ) {
            Mix_FreeChunk(  waves[i] );
             waves[i] = 0;
    }
	if ( audioOpen ) {
		Mix_CloseAudio();
		audioOpen = false;
	}

}

void

Sound::parse(XmlReader& reader)
{
    //Read from config
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();
        
        //check if Attribute handled by parent
        if(parseAttribute(attribute, value)) {
            continue;
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }
    // no more elements to parse
 
    //Load Sound
    audioOpen = false;
   	/* Open the audio device */
	if (Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
	} else {
		audioOpen = true;
        memset(&waves, 0, sizeof(waves));
        
        //Load Waves
        //
        //TODO: there has to be a better way to organize the Sounds.
        waves[ 0 ] = Mix_LoadWAV("data/sounds/Click.wav");
        waves[ 1 ] = Mix_LoadWAV("data/sounds/School3.wav");
        waves[ 3 ] = Mix_LoadWAV("data/sounds/RailTrain3.wav");
        waves[ 4 ] = Mix_LoadWAV("data/sounds/TraficLow3.wav");
	}
}


/*
 *  Playback an Audio-Effect
 */
void Sound::playwav( int id ) {
    printf("Audio %i request...", id);
    
    if( !audioOpen ){
		printf("Can't play Audio.\n");
        return;
    }
    if( waves[ id ] ) {
        Mix_PlayChannel(0, waves[ id ], 0); 
        printf("Oky\n");
    }
}

//Register as Component
IMPLEMENT_COMPONENT_FACTORY(Sound)
