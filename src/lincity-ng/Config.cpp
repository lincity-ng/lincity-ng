/* ---------------------------------------------------------------------- *
 * src/lincity-ng/Config.cpp
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

#include "Config.hpp"

#include <assert.h>                       // for assert
#include <cfgpath.h>                      // for MAX_PATH, get_user_config_file
#include <fmt/format.h>                   // for format
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlerror.h>              // for XML_ERR_OK
#include <libxml/xmlversion.h>            // for LIBXML_VERSION
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <limits.h>                       // for INT_MAX, INT_MIN
#include <stdio.h>                        // for sscanf, NULL
#include <stdlib.h>                       // for exit
#include <iostream>                       // for basic_ostream, operator<<
#include <memory>                         // for shared_ptr
#include <stdexcept>                      // for runtime_error

#include "config.h"                       // for PACKAGE_NAME, PACKAGE_VERSION
#include "lincity/world.hpp"              // for WORLD_SIDE_LEN
#include "util/xmlutil.hpp"               // for xmlStr, unexpectedXmlElement

template<typename V>
static std::optional<V> parseValue(const std::string& value);
template<>
/*static*/ std::optional<int> parseValue(const std::string& value);
template<>
/*static*/ std::optional<bool> parseValue(const std::string& value);
template<>
/*static*/ std::optional<std::string> parseValue(const std::string& value);
template<>
/*static*/ std::optional<std::filesystem::path> parseValue(
  const std::string& value);
static std::optional<int> validateRange(const std::optional<int>& value,
  int minValue = INT_MIN, int maxValue = INT_MAX);

template<typename V>
static void saveOption(xmlTextWriterPtr xmlWriter, const std::string& name,
  const Config::Option<V>& option);
template<>
/*static*/ void saveOption(xmlTextWriterPtr xmlWriter, const std::string& name,
  const Config::Option<int>& option);
template<>
/*static*/ void saveOption(xmlTextWriterPtr xmlWriter, const std::string& name,
  const Config::Option<bool>& option);
template<>
/*static*/ void saveOption(xmlTextWriterPtr xmlWriter, const std::string& name,
  const Config::Option<std::string>& option);
template<>
/*static*/ void saveOption(xmlTextWriterPtr xmlWriter, const std::string& name,
  const Config::Option<std::filesystem::path>& option);

Config *configPtr = nullptr;
Config *getConfig() {
  assert(configPtr);
  return configPtr;
}

Config::Config() {
  useOpenGL.default_ = false;
  useFullScreen.default_ = true;
  videoX.default_ = 1024;
  videoY.default_ = 768;

  soundVolume.default_ = 100;
  musicVolume.default_ = 50;
  soundEnabled.default_ = true;
  musicEnabled.default_ = true;
  musicTheme.default_ = "default";

  carsEnabled.default_ = true;
  worldSize.default_ = WORLD_SIDE_LEN;
  language.default_ = "autodetect";

  {
    char configFileStr[MAX_PATH];
    get_user_config_file(configFileStr, MAX_PATH, PACKAGE_NAME);
    if(*configFileStr)
      configFile.default_ = std::filesystem::path(configFileStr);
    else
      std::cerr << "warning: "
        << "failed to compute default config file location" << std::endl;
  }
  {
    char userDataDirStr[MAX_PATH];
    get_user_data_folder(userDataDirStr, MAX_PATH, PACKAGE_NAME);
    if(*userDataDirStr)
      userDataDir.default_ = std::filesystem::path(userDataDirStr);
    else
      std::cerr << "warning: "
        << "failed to compute default user data directory location"
        << std::endl;
  }
  appDataDir.default_ = std::filesystem::path(INSTALL_FULL_APPDATADIR);
}

Config::~Config() {}

void Config::load(std::filesystem::path configFile) {
  if(configFile.empty())
    configFile = this->configFile.get();
  if(!std::filesystem::exists(configFile)) {
    std::cerr << "info: config file does not exist: "
      << configFile.string() << std::endl;
    return;
  }

  xmlpp::TextReader xmlReader(configFile.string());
  if(!xmlReader.read())
    throw std::runtime_error("config file is empty");
  while(true) {
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element);
    else if(xmlReader.get_name() == "lc-config")
      break;
    else
      unexpectedXmlElement(xmlReader);
    if(!xmlReader.next())
      throw std::runtime_error("failed to find <lc-config> element");
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
          useOpenGL.config = parseValue<bool>(xml_val);
        else if(xml_tag == "x")
          videoX.config = validateRange(parseValue<int>(xml_val), 640);
        else if(xml_tag == "y")
          videoY.config = validateRange(parseValue<int>(xml_val), 480);
        else if(xml_tag == "fullscreen")
          useFullScreen.config = parseValue<bool>(xml_val);
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
          soundVolume.config = validateRange(parseValue<int>(xml_val), 0, 100);
        else if(xml_tag == "musicVolume")
          musicVolume.config = validateRange(parseValue<int>(xml_val), 0, 100);
        else if(xml_tag == "soundEnabled")
          soundEnabled.config = parseValue<bool>(xml_val);
        else if(xml_tag == "musicEnabled")
          musicEnabled.config = parseValue<bool>(xml_val);
        else if(xml_tag == "musicTheme")
          musicTheme.config = parseValue<std::string>(xml_val);
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
        if(xml_tag == "language")
          language.config = parseValue<std::string>(xml_val);
        else if(xml_tag == "WorldSideLen")
          worldSize.config = validateRange(parseValue<int>(xml_val), 50, 10000);
        else if(xml_tag == "carsEnabled")
          carsEnabled.config = parseValue<bool>(xml_val);
        else if(xml_tag == "appDataDir")
          appDataDir.config = parseValue<std::filesystem::path>(xml_val);
        else if(xml_tag == "userDataDir")
          userDataDir.config = parseValue<std::filesystem::path>(xml_val);
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
    configFile = this->configFile.get();

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
      saveOption(xmlWriter, "x", videoX);
      saveOption(xmlWriter, "y", videoY);
      saveOption(xmlWriter, "useOpenGL", useOpenGL);
      saveOption(xmlWriter, "fullscreen", useFullScreen);
    xmlTextWriterEndElement(xmlWriter);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"audio");
      saveOption(xmlWriter, "soundEnabled", soundEnabled);
      saveOption(xmlWriter, "soundVolume", soundVolume);
      saveOption(xmlWriter, "musicEnabled", musicEnabled);
      saveOption(xmlWriter, "musicVolume", musicVolume);
      saveOption(xmlWriter, "musicTheme", musicTheme);
    xmlTextWriterEndElement(xmlWriter);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"game");
      saveOption(xmlWriter, "language", language);
      saveOption(xmlWriter, "WorldSideLen", worldSize);
      saveOption(xmlWriter, "carsEnabled", carsEnabled);
      saveOption(xmlWriter, "appDataDir", appDataDir);
      saveOption(xmlWriter, "userDataDir", userDataDir);
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
      if(configFile.session)
        throw std::runtime_error("--config may be specified only once");
      configFile.session = std::filesystem::path(argv[argi]);
    }
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
      useOpenGL.session = true;
    } else if(argStr == "-s" || argStr == "--sdl") {
      useOpenGL.session = false;
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
      videoX.session = newX;
      videoY.session = newY;
    } else if(argStr == "-f" || argStr == "--fullscreen") {
      useFullScreen.session = true;
    } else if(argStr == "-w" || argStr == "--window") {
      useFullScreen.session = false;
    } else if(argStr == "-m" || argStr == "--mute") {
      soundEnabled.session = false;
      musicEnabled.session = false;
    } else if(argStr == "--config" || argStr == "-c") {
      argi++;
      if(argi >= argc)
        throw std::runtime_error("--config needs a parameter");
      configFile.session = std::filesystem::path(argv[argi]);
    } else if(argStr == "--app-data") {
      argi++;
      if(argi >= argc)
        throw std::runtime_error("--app-data needs a parameter");
      appDataDir.session = std::filesystem::path(argv[argi]);
    } else if(argStr == "--user-data") {
      argi++;
      if(argi >= argc)
        throw std::runtime_error("--user-data needs a parameter");
      userDataDir.session = std::filesystem::path(argv[argi]);
    } else {
      std::cerr << "Unknown command line argument: " << argStr << "\n";
      exit(1);
    }
  }

  load(configFile.get());

#ifdef DEBUG
  std::cerr << "  config file: " << configFile.get()  << std::endl
            << " app data dir: " << appDataDir.get()  << std::endl
            << "user data dir: " << userDataDir.get() << std::endl;
#endif

  if(!std::filesystem::is_directory(appDataDir.get())) {
    std::cerr << "error: app data location is not a directory: "
      << appDataDir.get().string() << std::endl
      << "  Use `--app-data` to set the correct app data location."
      << " Otherwise, LinCity-NG will likely crash." << std::endl;
  }

  if(!std::filesystem::is_directory(userDataDir.get())) {
    std::cerr << "error: user data location is not a directory: "
      << userDataDir.get().string() << std::endl
      << "  Use `--user-data` to set the correct user data location."
      << std::endl;
  }

#ifdef DISABLE_GL_MODE
  if(useOpenGL.get()) {
    useOpenGL.session = false;
    std::cerr << "warning: GL mode was requested, but it is disabled for this"
      " build. Using SDL mode instead." << std::endl;
  }
#endif
}


template class Config::Option<int>;
template class Config::Option<bool>;
template class Config::Option<std::string>;
template class Config::Option<std::filesystem::path>;

template<typename T>
Config::Option<T>::Option() :
  default_(std::nullopt)
{ }

template<typename T>
Config::Option<T>::Option(const T& default_) :
  default_(default_)
{ }

template<typename T>
const T&
Config::Option<T>::get() const {
  if(session) return session.value();
  else if(config) return config.value();
  else if(default_) return default_.value();
  else {
    throw std::runtime_error("this config option has no default");
  }
}

template<typename T>
bool Config::Option<T>::isDefault() const {
  return !session && !config;
}

template<typename T>
void
Config::Option<T>::sessionToConfig() {
  config = session;
}

template<>
/*static*/ std::optional<int>
parseValue(const std::string& value) {
  if(value == "default")
    return std::nullopt;

  int tmp;
  if(sscanf(value.c_str(), "%i", &tmp) != 1) {
    throw std::runtime_error(fmt::format(
      "error: failed to parse integer value {:?}", value));
  }
  return tmp;
}

template<>
/*static*/ std::optional<bool>
parseValue(const std::string& value) {
  if(value == "default")
    return std::nullopt;

  if(value == "no" || value == "NO"
    || value == "n" || value == "N"
    || value == "off" || value == "OFF"
    || value == "false" || value == "FALSE"
    || value == "0"
  ) {
    return false;
  }
  if(value == "yes" || value == "YES"
    || value == "y" || value == "Y"
    || value == "on" || value == "ON"
    || value == "true" || value == "TRUE"
    || value == "1"
  ) {
    return true;
  }

  throw std::runtime_error(fmt::format(
    "error: failed to parse boolean value {:?} (should be \"yes\" or \"no\")",
    value));
}

template<>
/*static*/ std::optional<std::string>
parseValue(const std::string& value) {
  if(value == "default")
    return std::nullopt;
  return value;
}

template<>
/*static*/ std::optional<std::filesystem::path>
parseValue(const std::string& value) {
  if(value == "default")
    return std::nullopt;
  return std::filesystem::path(value);
}

static std::optional<int>
validateRange(const std::optional<int>& value,
  int minValue /*= INT_MIN*/, int maxValue /*= INT_MAX*/
) {
  if(value && (*value < minValue || *value > maxValue)) {
    throw std::runtime_error(fmt::format(
      "error: value {:d} is outside range {:d}..{:d}",
      *value, minValue, maxValue));
  }
  return value;
}

template<>
/*static*/ void
saveOption(xmlTextWriterPtr xmlWriter, const std::string& name,
  const Config::Option<int>& option
) {
  if(option.config)
    xmlTextWriterWriteElement(xmlWriter, (xmlStr)name.c_str(),
      xmlFormat<int>(*option.config));
  else if(option.default_)
    xmlTextWriterWriteElement(xmlWriter, (xmlStr)name.c_str(),
      (xmlStr)"default");
}

template<>
/*static*/ void
saveOption(xmlTextWriterPtr xmlWriter, const std::string& name,
  const Config::Option<bool>& option
) {
  if(option.config)
    xmlTextWriterWriteElement(xmlWriter, (xmlStr)name.c_str(),
      xmlFormat<std::string>(*option.config?"yes":"no"));
  else if(option.default_)
    xmlTextWriterWriteElement(xmlWriter, (xmlStr)name.c_str(),
      (xmlStr)"default");
}

template<>
/*static*/ void
saveOption(xmlTextWriterPtr xmlWriter, const std::string& name,
  const Config::Option<std::string>& option
) {
  if(option.config)
    xmlTextWriterWriteElement(xmlWriter, (xmlStr)name.c_str(),
      xmlFormat<std::string>(option.config->c_str()));
  else if(option.default_)
    xmlTextWriterWriteElement(xmlWriter, (xmlStr)name.c_str(),
      (xmlStr)"default");
}

template<>
/*static*/ void
saveOption(xmlTextWriterPtr xmlWriter, const std::string& name,
  const Config::Option<std::filesystem::path>& option
) {
  if(option.config)
    xmlTextWriterWriteElement(xmlWriter, (xmlStr)name.c_str(),
      xmlFormat<std::string>(option.config->string().c_str()));
  else if(option.default_)
    xmlTextWriterWriteElement(xmlWriter, (xmlStr)name.c_str(),
      (xmlStr)"default");
}

/** @file lincity-ng/Config.cpp */
