/**
 *  Sound Component for Lincity-NG.
 *   
 *  February 2005, Wolfgang Becker <uafr@gmx.de>
 *
 *  20050224
 *  sound as own class
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
    for (chunks_t::iterator i = waves.begin(); i != waves.end(); i++) {
        Mix_FreeChunk( i->second );
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
        //Load Waves

/*  waves[ 0 ] = Mix_LoadWAV("data/sounds/Click.wav");
-        waves[ 1 ] = Mix_LoadWAV("data/sounds/School3.wav");
-        waves[ 3 ] = Mix_LoadWAV("data/sounds/RailTrain3.wav");
-        waves[ 4 ] = Mix_LoadWAV("data/sounds/TraficLow3.wav");
*/
        std::string filename = "School3.wav";
        Mix_Chunk *chunk;
        chunk =  Mix_LoadWAV( ("data/sounds/" + filename).c_str() );
        if (chunk) {
            std::string idName = getIdName( filename );
            std::cout << "Lade" << filename << " als "<< idName << "\n"; 
            waves.insert( std::pair<std::string,Mix_Chunk*>(idName, chunk) );
        }
        else
        {
            std::cout << "Couldn't load '" << filename << "'\n";
        }
	}
}


/*
 *  Playback an Audio-Effect.
 *  Name is the Name of an Audiofile from sounds/ minus .wav
 *  and without trailing Numbers. If there are eg.
 *  beep1.wav, beep2.wav, beep3.wav
 *  playwav( "beep" ) would pick one of the three FIles randomly
 */
void Sound::playwav( const std::string name ) {
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

//Register as Component
IMPLEMENT_COMPONENT_FACTORY(Sound)
