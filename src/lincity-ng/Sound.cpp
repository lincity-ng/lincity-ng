/* ---------------------------------------------------------------------- *
 * src/lincity-ng/Sound.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Wolfgang Becker <uafr@gmx.de>
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "Sound.hpp"

#include <SDL.h>                          // for SDL_GetError, SDL_CreateThread
#include <SDL_mixer.h>                    // for Mix_Volume, Mix_FreeMusic
#include <fmt/base.h>                     // for println
#include <fmt/format.h>                   // for format
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <stdio.h>                        // for stderr, size_t, fprintf, NULL
#include <stdlib.h>                       // for rand
#include <cassert>                        // for assert
#include <cmath>                          // for round
#include <iostream>                       // for basic_ostream, operator<<
#include <optional>                       // for optional
#include <stdexcept>                      // for runtime_error
#include <utility>                        // for pair
#include <vector>                         // for vector
#include <fmt/std.h> // IWYU pragma: keep

#include "Config.hpp"                     // for getConfig, Config
#include "lincity/lin-city.hpp"           // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"           // for Construction
#include "lincity/resources.hpp"          // for ResourceGroup
#include "lincity/world.hpp"              // for MapTile
#include "util/xmlutil.hpp"               // for xmlParse, unexpectedXmlAttr...

Sound* soundPtr = 0;

Sound *getSound()
{
  return soundPtr;
}

int
Sound::soundThread(void* ptr)
{
    Sound* sound = (Sound*) ptr;
    sound->loadWaves();
    return 0;
}

void
Sound::loadWaves() {
  // TODO: improve the format of sounds.xml

  std::filesystem::path directory = getConfig()->appDataDir.get() / "sounds";
  std::filesystem::path xmlfile = directory / "sounds.xml";
  xmlpp::TextReader reader(xmlfile);
  if(!reader.read())
    throw std::runtime_error(fmt::format("file is empty: {}", xmlfile));
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
    if(!reader.next())
      throw std::runtime_error(
        fmt::format("file doesn't contain XML data: {}", xmlfile));
  }

  std::filesystem::path filename;
  std::filesystem::path fullname;
  std::vector<ResourceGroup*> resGrpVec;
  resGrpVec.clear();
  int resourceID_level = 0;
  std::filesystem::path key;
  Mix_Chunk *chunk;

  if(!reader.is_empty_element())
  while(reader.read()) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      continue;
    }
    xmlpp::ustring element = reader.get_name();
    if(element == "resourceID") {
      std::string resName;

      while(reader.move_to_next_attribute()) {
        xmlpp::ustring name = reader.get_name();
        xmlpp::ustring value = reader.get_value();
        if(name == "name")
          resName = xmlParse<std::string>(value);
        else
          unexpectedXmlAttribute(reader);
      }
      reader.move_to_element();

      if(ResourceGroup::resMap.count(resName))
      {
        resGrpVec.push_back(ResourceGroup::resMap[resName]);
        resourceID_level = reader.get_depth();
        if(resGrpVec.back()->sounds_loaded) //could crash if game is already running
          fmt::println(stderr,
            "warning: duplicate resourceID in sounds.xml: {:?}", resName);
      }
      else
        fmt::println(stderr,
          "warning: unknown resourceID in sounds.xml: {:?}", resName);
    }
    if(reader.get_depth() < resourceID_level-1) {
      for(size_t i=0; i< resGrpVec.size(); ++i)
        resGrpVec[i]->sounds_loaded = true;
      resGrpVec.clear();
      resourceID_level = 0;
    }
    if(element == "sound") {
      while(reader.move_to_next_attribute()) {
        xmlpp::ustring name = reader.get_name();
        xmlpp::ustring value = reader.get_value();
        if(name == "file")
          key = xmlParse<std::filesystem::path>(value);
        else
          unexpectedXmlAttribute(reader);
      }
      reader.move_to_element();

      fullname = directory / key;
      chunk = Mix_LoadWAV(fullname.string().c_str());
      if(!chunk) {
        fmt::println(stderr, "warning: filed to load sound {:?}: {}",
          key, Mix_GetError());
      }
      if (resourceID_level && resGrpVec.size()) {
        for(size_t i=0; i< resGrpVec.size(); ++i)
          resGrpVec[i]->chunks.push_back(chunk);
      }
      else {
        std::string idName = getIdName(key);
        waves.insert(std::pair<std::string,Mix_Chunk*>(idName, chunk));
      }
      key.clear();
    }
  }

  if(resGrpVec.size()) {
    for(size_t i=0; i< resGrpVec.size(); ++i)
      resGrpVec[i]->sounds_loaded = true;
    resGrpVec.clear();
  }
}

 /*
 * Load music theme from subfolder of 'music/'.
 */
void Sound::loadMusicTheme() {
  std::filesystem::path musicDir = getConfig()->appDataDir.get() / "music";
  //Reset track counter:
  totalTracks=0;
  playlist.clear();
  //Get the current music theme
  std::string theme = getConfig()->musicTheme.get();
  std::filesystem::path themeDir = musicDir / theme;
  std::filesystem::path xml_name = themeDir / (theme + ".xml");

  if(!std::filesystem::is_regular_file(xml_name))
    throw std::runtime_error(
      std::string("could not load music theme: " + theme));

  //Get the number of songs
  xmlpp::TextReader reader(xml_name);
  if(!reader.read())
    throw std::runtime_error(fmt::format("file is empty: {}", xml_name));
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
    if(!reader.next())
      throw std::runtime_error(
        fmt::format("file doesn't contain XML data: {}", xml_name));
  }

  if(!reader.is_empty_element() && reader.read())
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      reader.next();
      continue;
    }
    xmlpp::ustring element = reader.get_name();

    if(element == "song") {
      std::string title;
      std::filesystem::path filename;
      float lowest_tech_level = 0.0;
      float highest_tech_level = 10000.0;

      while(reader.move_to_next_attribute()) {
        xmlpp::ustring name = reader.get_name();
        xmlpp::ustring value = reader.get_value();
        if(name == "title")
          title = xmlParse<std::string>(value);
        else if(name == "filename")
          filename = themeDir / xmlParse<std::filesystem::path>(value);
        else if(name == "highest-tech-level")
          highest_tech_level = xmlParse<float>(value);
        else if(name == "lowest-tech-level")
          lowest_tech_level = xmlParse<float>(value);
        else
          unexpectedXmlAttribute(reader);
      }
      reader.move_to_element();

      song tempSong;
      tempSong.title = title;
      tempSong.filename = filename;
      tempSong.trackNumber = totalTracks;
      tempSong.lowestTechLevel = lowest_tech_level;
      if(highest_tech_level == 0)
        highest_tech_level = 10 * MAX_TECH_LEVEL;
      tempSong.highestTechLevel = highest_tech_level;

      playlist.push_back(tempSong);
      totalTracks++;
    }
    else {
      unexpectedXmlElement(reader);
    }
    reader.next();
  }

  while(reader.next()) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element)
      continue;
    unexpectedXmlElement(reader);
  }
}

Sound::Sound()
    : currentMusic(0)
{
    assert( soundPtr == 0);
    soundPtr = this;
    loaderThread = 0;

    //Load Sound
    audioOpen = false;
    /* Open the audio device */
    if (Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        return;
    } else {
        audioOpen = true;
        loaderThread = SDL_CreateThread(soundThread, "Sound", this);
    }

    setMusicVolume(getConfig()->musicVolume.get());
    setSoundVolume(getConfig()->soundVolume.get());

    totalTracks = 0;
    loadMusicTheme();

    //'totalTracks' gets too high value in while loop. Let's fix it.
    totalTracks = totalTracks-1;

    //Load background music.
    //First check if there really is something in playlist to prevent crashing
    if(!playlist.empty()) {
        currentTrack = playlist[0];
    }
    playMusic();
}

Sound::~Sound()
{
    //SDL_KillThread( loaderThread );
    SDL_WaitThread( loaderThread, NULL );
    if(currentMusic)
        Mix_FreeMusic(currentMusic);

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
 *  playSound( "beep" ) would pick one of the three Files randomly
 */
void
Sound::playSound(const std::string& name) {
  if(!getConfig()->soundEnabled.get()) {
    return;
  }
  if(!audioOpen) {
    return;
  }

  chunks_t::size_type count = waves.count( name );
  if(count == 0) {
    std::cout << "Couldn't find audio file '" << name << "'" << std::endl;
    return;
  }

  chunks_t::iterator it = waves.find(name);
  for(int i = rand() % count; i > 0; i--) {
    it++;
  }

  Mix_Volume(0, getConfig()->soundVolume.get());
  Mix_PlayChannel( 0, it->second, 0 );
}

void
Sound::playSound(const MapTile& tile) {
  ResourceGroup* resourceGroup;
  if(tile.reportingConstruction)
    resourceGroup = tile.reportingConstruction->soundGroup;
  else
    resourceGroup = tile.getTileResourceGroup();
  int count = resourceGroup->chunks.size();
  if(count)
    playASound(resourceGroup->chunks[rand() % count]);
}

void Sound::playASound(Mix_Chunk *chunk) {
  if(!getConfig()->soundEnabled.get())
    return;
  if(!audioOpen)
    return;
  Mix_Volume(0, getConfig()->soundVolume.get());
  Mix_PlayChannel(0, chunk, 0);
}


/*
 * Get ID-String for a given Filename.
 */
std::string
Sound::getIdName(const std::string& filename)
{
    std::string::size_type pos = filename.find_first_of(".0123456789");

    return filename.substr(0, pos);
}

/*
 * Change backround music.
 * Possible variables can be found from Sound.hpp: enum musicTransport
 * Currently there are three of them: NEXT_TRACK, NEXT_OR_FIRST_TRACK, PREV_TRACK
 * Sould be self-explanatory.
 */
void
Sound::changeTrack(MusicTransport command)
{

    //Something may gone wrong in the initialization:
    if(playlist.empty())
        return;

    switch(command) {
        case NEXT_TRACK:
            if (currentTrack.trackNumber+1 <= totalTracks) {
                currentTrack=playlist[currentTrack.trackNumber+1];
                playMusic();
            }
            break;

        case NEXT_OR_FIRST_TRACK:
            if (currentTrack.trackNumber+1 <= totalTracks) {
                currentTrack=playlist[currentTrack.trackNumber+1];
                playMusic();
            } else {
                //Jump to the beginning
                currentTrack=playlist[0];
                playMusic();
            }
            break;

        case PREV_TRACK:
            if (currentTrack.trackNumber > 0) {
                currentTrack=playlist[currentTrack.trackNumber-1];
                playMusic();
            } else {
                //Jump to the beginning
                currentTrack=playlist[0];
                playMusic();
            }
            break;
    }

}


void
Sound::playMusic()
{
    if(!audioOpen)
        return;

    if(getConfig()->musicEnabled.get()) {

        if(currentMusic)
        {
            if(Mix_PlayingMusic())
            {   Mix_FreeMusic(currentMusic);}
            currentMusic = 0;
        }

        //Check if current track is allowed at this tech level
        //This calculates the right tech_level and rounds it by one decimal.
        float current_tech = tech_level * (float)100 / MAX_TECH_LEVEL;
        current_tech = round(current_tech*10)/10;

        if(current_tech < currentTrack.lowestTechLevel
          || current_tech > currentTrack.highestTechLevel
        ) {
            //std::cerr << "Next track is " << currentTrack.title
            //<< " and it's tech level prerequisites range from "
            //<< currentTrack.lowestTechLevel << " to " << currentTrack.highestTechLevel << "." << std::endl;
            //std::cerr << "Current tech level is " << current_tech << "." << std::endl;
            changeTrack(NEXT_OR_FIRST_TRACK);
            return;
        }

        currentMusic = Mix_LoadMUS(currentTrack.filename.string().c_str());
        if(currentMusic == 0) {
            std::cerr << "Couldn't load music file '"
                << currentTrack.filename << "': "
                << SDL_GetError() << std::endl;
            return;
        }

        Mix_PlayMusic(currentMusic, 1);
    }
}

void
Sound::enableMusic(bool enabled) {
  bool old = getConfig()->musicEnabled.get();
  getConfig()->musicEnabled.session = enabled;

  if(old == enabled)
    return;

  if(!audioOpen)
    return;

  if(enabled) {
    playMusic();
  } else {
    if(Mix_PlayingMusic()) {
      Mix_FadeOutMusic(1000);
    }
  }
}

void
Sound::setMusicVolume(int vol)
{
    assert(vol >= 0 && vol <= 100);
    getConfig()->musicVolume.session = vol;
    float volvalue = vol * MIX_MAX_VOLUME / 100.0;
    Mix_VolumeMusic(static_cast<int>(volvalue));
}

void
Sound::setSoundVolume(int vol)
{
    assert(vol >= 0 && vol <= 100);
    getConfig()->soundVolume.session = vol;
    float volvalue = vol * MIX_MAX_VOLUME / 100.0;
    Mix_Volume(-1, static_cast<int>(volvalue));
}


/** @file lincity-ng/Sound.cpp */
