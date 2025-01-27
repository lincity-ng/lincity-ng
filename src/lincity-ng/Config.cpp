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

#include "Config.hpp"

#include <assert.h>                       // for assert
#include <cfgpath.h>                      // for MAX_PATH, get_user_config_file
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlerror.h>              // for XML_ERR_OK
#include <libxml/xmlversion.h>            // for LIBXML_VERSION
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteFormatEle...
#include <limits.h>                       // for INT_MAX, INT_MIN
#include <stdio.h>                        // for sscanf, NULL
#include <stdlib.h>                       // for exit
#include <iostream>                       // for basic_ostream, operator<<
#include <memory>                         // for shared_ptr
#include <stdexcept>                      // for runtime_error

#include "config.h"                       // for PACKAGE_NAME, PACKAGE_VERSION
#include "lincity/engglobs.h"             // for world
#include "lincity/world.h"                // for World, WORLD_SIDE_LEN
#include "lincity/xmlloadsave.h"          // for xmlStr, unexpectedXmlElement


static int parseInt(const std::string& value, int defaultValue,
  int minValue = INT_MIN, int maxValue = INT_MAX);
static bool parseBool(const std::string& value, bool defaultValue);

Config *configPtr = nullptr;
Config *getConfig() {
  assert(configPtr);
  return configPtr;
}

Config::Config() {
  useOpenGL = true;  //OpenGL is often way too slow
  useFullScreen = true;
  videoX = 1024;
  videoY = 768;
  restartOnChangeScreen = false;

  soundVolume = 100;
  musicVolume = 50;
  soundEnabled = true;
  musicEnabled = true;
  musicTheme = "default";

  carsEnabled = true;
  language = "autodetect";
  appDataDirIsDefault = false;
  userDataDirIsDefault = false;
  // TODO: remove this. also see issue #225
  world.len(WORLD_SIDE_LEN);
  // TODO: Remove monthgraph size from config. Monthgraph size should be based
  //       on available space in GUI.
  monthgraphW = 190;
  monthgraphH = 64;
}

Config::~Config() {}

void Config::load(std::filesystem::path configFile) {
  if(configFile.empty())
    configFile = this->configFile;
  if(!std::filesystem::exists(configFile)) {
    std::cerr << "info: config file does not exist: "
      << appDataDir.string() << std::endl;
    return;
  }

  xmlpp::TextReader xmlReader(configFile.string());
  xmlReader.read();
  while(true) {
    if(xmlReader.get_read_state() == xmlpp::TextReader::ReadState::EndOfFile)
      throw std::runtime_error("failed to find lc-config element");
    else if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element);
    else if(xmlReader.get_name() == "lc-config")
      break;
    else
      unexpectedXmlElement(xmlReader);
    xmlReader.next();
  }

  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    if(xmlReader.get_name() == "video") {
      if(!xmlReader.is_empty_element() && xmlReader.read())
      while(
        xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement
      ) {
        if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
          xmlReader.next();
          continue;
        }

        std::string xml_tag = xmlReader.get_name();
        std::string xml_val = xmlReader.read_inner_xml();
        if(xml_tag == "useOpenGL")
          useOpenGL = parseBool(xml_val, useOpenGL);
        else if(xml_tag == "x")
          videoX = parseInt(xml_val, videoX, 640);
        else if(xml_tag == "y")
          videoY = parseInt(xml_val, videoY, 480);
        else if(xml_tag == "fullscreen")
          useFullScreen = parseBool(xml_val, useFullScreen);
        else if(xml_tag == "restartOnChangeScreen")
          restartOnChangeScreen = parseBool(xml_val, restartOnChangeScreen);
        else
          unexpectedXmlElement(xmlReader);

        xmlReader.next();
      }
    }
    else if(xmlReader.get_name() == "audio") {
      if(!xmlReader.is_empty_element() && xmlReader.read())
      while(
        xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement
      ) {
        if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
          xmlReader.next();
          continue;
        }

        std::string xml_tag = xmlReader.get_name();
        std::string xml_val = xmlReader.read_inner_xml();
        if(xml_tag == "soundVolume")
          soundVolume = parseInt(xml_val, soundVolume, 0, 100);
        else if(xml_tag == "musicVolume")
          musicVolume = parseInt(xml_val, musicVolume, 0, 100);
        else if(xml_tag == "soundEnabled")
          soundEnabled = parseBool(xml_val, soundEnabled);
        else if(xml_tag == "musicEnabled")
          musicEnabled = parseBool(xml_val, musicEnabled);
        else if(xml_tag == "musicTheme")
          musicTheme = xml_val;
        else
          unexpectedXmlElement(xmlReader);

        xmlReader.next();
      }
    }
    else if(xmlReader.get_name() == "game") {
      if(!xmlReader.is_empty_element() && xmlReader.read())
      while(
        xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement
      ) {
        if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
          xmlReader.next();
          continue;
        }

        std::string xml_tag = xmlReader.get_name();
        std::string xml_val = xmlReader.read_inner_xml();
        if(xml_tag == "monthgraphW")
          monthgraphW = parseInt(xml_val, monthgraphW, 0);
        else if(xml_tag == "monthgraphH")
          monthgraphH = parseInt(xml_val, monthgraphH, 0);
        else if(xml_tag == "language")
          language = xml_val;
        else if(xml_tag == "WorldSideLen")
          world.len(parseInt(xml_val, WORLD_SIDE_LEN, 50, 10000));
        else if(xml_tag == "carsEnabled")
          carsEnabled = parseBool(xml_val, carsEnabled);
        else if(xml_tag == "appDataDir")
          appDataDir = std::filesystem::path(xml_val);
        else if(xml_tag == "userDataDir")
          userDataDir = std::filesystem::path(xml_val);
        else
          unexpectedXmlElement(xmlReader);

        xmlReader.next();
      }
    }
    else {
      unexpectedXmlElement(xmlReader);
    }
    xmlReader.next();
  }

  while(xmlReader.next()) {
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element)
      continue;
    unexpectedXmlElement(xmlReader);
  }
}

void
Config::save(std::filesystem::path configFile) {
  if(configFile.empty())
    configFile = this->configFile;

  int xmlStatus = XML_ERR_OK;
  {
  xmlTextWriterPtr xmlWriter =
    xmlNewTextWriterFilename(configFile.string().c_str(), false);
  if(!xmlWriter) {
    throw std::runtime_error("failed to create XML text writer");
  }
  std::shared_ptr<xmlTextWriter> xmlWriterCloser(xmlWriter,
    [&xmlStatus](xmlTextWriterPtr xmlWriter) {
      #if LIBXML_VERSION >= 21300
        xmlStatus = xmlTextWriterClose(xmlWriter);
      #endif
      xmlFreeTextWriter(xmlWriter);
    }
  );

  xmlTextWriterSetIndent(xmlWriter, true);
  xmlTextWriterSetIndentString(xmlWriter, (xmlStr)"  ");

  xmlTextWriterStartDocument(xmlWriter, NULL, NULL, NULL);
  xmlTextWriterStartElement(xmlWriter, (xmlStr)"lc-config");
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"video");
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"x", "%d", videoX);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"y", "%d", videoY);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"useOpenGL", "%s",
        useOpenGL?"yes":"no");
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"fullscreen", "%s",
        useFullScreen?"yes":"no");
      xmlTextWriterWriteFormatElement(xmlWriter,
        (xmlStr)"restartOnChangeScreen", "%s",
        restartOnChangeScreen?"yes":"no");
    xmlTextWriterEndElement(xmlWriter);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"audio");
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"soundEnabled", "%s",
        soundEnabled?"yes":"no");
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"soundVolume", "%d",
        soundVolume);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"musicEnabled", "%s",
        musicEnabled?"yes":"no");
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"musicVolume", "%d",
        musicVolume);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"musicTheme", "%s",
        musicTheme.c_str());
    xmlTextWriterEndElement(xmlWriter);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"game");
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"language", "%s",
        language.c_str());
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"WorldSideLen", "%d",
        (world.len()<50)?50:world.len());
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"carsEnabled", "%s",
        carsEnabled?"yes":"no");
      if(!appDataDirIsDefault)
        xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"appDataDir", "%s",
          appDataDir.c_str());
      if(!userDataDirIsDefault)
        xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"userDataDir", "%s",
          userDataDir.c_str());
    xmlTextWriterEndElement(xmlWriter);
  xmlTextWriterEndElement(xmlWriter);
  xmlTextWriterEndDocument(xmlWriter);
  }
  if(xmlStatus) {
    throw std::runtime_error(
      std::string("XML parser error: ") + std::to_string(xmlStatus));
  }
}

void
Config::parseCommandLine(int argc, char** argv) {
  // first check for --config option
  for(int argi = 1; argi < argc; argi++) {
    std::string argStr = argv[argi];
    if(argStr == "--config" || argStr == "-c") {
      argi++;
      if(argi >= argc)
        throw std::runtime_error("--config needs a parameter");
      configFile = std::filesystem::path(argv[argi]);
      load(configFile);
    }
  }
  if(configFile.empty()) {
    char configFileStr[MAX_PATH];
    get_user_config_file(configFileStr, MAX_PATH, PACKAGE_NAME);
    configFile = std::filesystem::path(configFileStr);
    load(configFile);
  }

  for(int argi = 1; argi < argc; ++argi) {
    std::string argStr = argv[argi];

    if(argStr == "-v" || argStr == "--version") {
      std::cout << PACKAGE_NAME << " version " << PACKAGE_VERSION << "\n";
      exit(0);
    } else if(argStr == "-h" || argStr == "--help") {
      std::cout << PACKAGE_NAME << " version " << PACKAGE_VERSION << "\n";
      std::cout << "Command line overrides configfiles.\n";
      std::cout << "Known arguments are:\n";
      std::cout << "-v           --version         show version and exit\n";
      std::cout << "-h           --help            show this text and exit\n";
      std::cout << "-g           --gl              use OpenGL\n";
      std::cout << "-s           --sdl             use SDL\n";
      std::cout << "-S <size>    --size <size>     specify screensize (eg. -S 1024x768)\n";
      std::cout << "-w           --window          run in window\n";
      std::cout << "-f           --fullscreen      run fullscreen\n";
      std::cout << "-m           --mute            mute audio\n";
      std::cout << "-c <file>    --config <file>   configuration file location\n";
      std::cout << "             --app-data <dir>  app data location\n";
      std::cout << "             --user-data <dir> user data location\n";
      exit(0);
    } else if(argStr == "-g" || argStr == "--gl") {
      useOpenGL = true;
    } else if(argStr == "-s" || argStr == "--sdl") {
      useOpenGL = false;
    } else if(argStr == "-S" || argStr == "--size") {
      argi++;
      if(argi >= argc)
        throw std::runtime_error("--size needs a parameter");
      argStr = argv[argi];
      int newX, newY, count;
      count = sscanf( argStr.c_str(), "%ix%i", &newX, &newY );
      if( count != 2  ) {
        std::cerr << "Error: Can not parse --size parameter.\n";
        exit( 1 );
      }
      if(newX <= 0 || newY <= 0) {
        std::cerr << "Error: Size parameter out of range.\n";
        exit(1);
      }
      videoX = newX;
      videoY = newY;
    } else if(argStr == "-f" || argStr == "--fullscreen") {
      useFullScreen = true;
    } else if(argStr == "-w" || argStr == "--window") {
      useFullScreen = false;
    } else if(argStr == "-m" || argStr == "--mute") {
      soundEnabled = false;
      musicEnabled = false;
    } else if(argStr == "--app-data") {
      argi++;
      if(argi >= argc)
        throw std::runtime_error("--app-data needs a parameter");
      appDataDir = std::filesystem::path(argv[argi]);
    } else if(argStr == "--user-data") {
      argi++;
      if(argi >= argc)
        throw std::runtime_error("--user-data needs a parameter");
      userDataDir = std::filesystem::path(argv[argi]);
    } else {
      std::cerr << "Unknown command line argument: " << argStr << "\n";
      exit(1);
    }
  }

  if(appDataDir.empty()) {
    appDataDir = std::filesystem::path(INSTALL_FULL_APPDATADIR);
    appDataDirIsDefault = true;
  }

  if(userDataDir.empty()) {
    char userDataDirStr[MAX_PATH];
    get_user_data_folder(userDataDirStr, MAX_PATH, PACKAGE_NAME);
    userDataDir = std::filesystem::path(userDataDirStr);
    userDataDirIsDefault = true;
  }

#ifdef DEBUG
  std::cerr << "  config file: " << configFile  << std::endl
            << " app data dir: " << appDataDir  << std::endl
            << "user data dir: " << userDataDir << std::endl;
#endif

  if(!std::filesystem::is_directory(appDataDir)) {
    std::cerr << "error: app data location is not a directory: "
      << appDataDir.string() << std::endl
      << "  Use `--app-data` to configure the correct app data location."
      << " Otherwise, LinCity-NG will likely crash." << std::endl;
  }

  if(!std::filesystem::is_directory(userDataDir)) {
    std::cerr << "error: user data location is not a directory: "
      << userDataDir.string() << std::endl
      << "  Use `--user-data` to configure the correct user data location."
      << std::endl;
  }
}

static int parseInt(const std::string& value,
  int defaultValue, int minValue, int maxValue
) {
    int tmp;
    if(sscanf(value.c_str(), "%i", &tmp) != 1)
    {
        std::cerr << "Config::parseInt# Error parsing integer value '" << value << "'.\n";
        tmp = defaultValue;
    }
    if( ( tmp >= minValue ) && ( tmp <= maxValue ) ) {
        return tmp;
    } else {
        std::cerr << "Config::parseInt# Value '" << value << "' not in ";
        std::cerr << minValue << ".." << maxValue << "\n";
        return defaultValue;
    }
}

static bool parseBool(const std::string& value, bool defaultValue) {
    if(value == "no" || value == "off"
            || value == "false" || value == "NO"
            || value == "OFF" || value == "FALSE") {
        return false;
    }
    if(value == "yes" || value == "on"
            || value == "true" || value == "YES"
            || value == "ON" || value == "TRUE") {
        return true;
    }

    std::cerr << "Couldn't parse boolean value '" << value << "'.\n";
    return defaultValue;
}

/** @file lincity-ng/Config.cpp */
