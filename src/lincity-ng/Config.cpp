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

#include <cfgpath.h>                      // for MAX_PATH, get_user_config_file
#include <fmt/base.h>                     // for println
#include <fmt/format.h>                   // for format
#include <fmt/std.h> // IWYU pragma: keep
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <libxml/xmlerror.h>              // for XML_ERR_OK
#include <libxml/xmlversion.h>            // for LIBXML_VERSION
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <stdlib.h>                       // for getenv
#include <cassert>                        // for assert
#include <climits>                        // for INT_MAX, INT_MIN
#include <cstdio>                         // for stderr, NULL, sscanf
#include <iostream>                       // for basic_ostream, endl, operat...
#include <memory>                         // for shared_ptr
#include <stdexcept>                      // for runtime_error

#include "config.h"                       // for PACKAGE_NAME, INSTALL_FULL_...
#include "lincity/world.hpp"              // for WORLD_SIDE_LEN
#include "util/gettextutil.hpp"           // for _
#include "util/xmlutil.hpp"               // for xmlStrF, xmlStr, xmlFormat

#ifdef LINCITYNG_RELOCATABLE
#include <SDL.h>                          // for SDL_GetBasePath
#endif

template<typename V>
static std::optional<V> xmlParseConfig(const xmlpp::ustring& s);
template<typename V>
static xmlStrF xmlFormatConfig(const std::optional<V>& option);
static std::optional<int> validateRange(const std::optional<int>& value,
  int minValue = INT_MIN, int maxValue = INT_MAX);

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
  showVersion.default_ = false;
  showHelp.default_ = false;

  soundVolume.default_ = 100;
  musicVolume.default_ = 50;
  soundEnabled.default_ = true;
  musicEnabled.default_ = true;
  musicTheme.default_ = "default";

  carsEnabled.default_ = true;
  worldSize.default_ = WORLD_SIDE_LEN;
  language.default_ = "autodetect";

  { // configFile.default_
    char configFileStr[MAX_PATH];
    get_user_config_file(configFileStr, MAX_PATH, PACKAGE_NAME);
    if(*configFileStr)
      configFile.default_ = std::filesystem::path(configFileStr);
    else
      fmt::println(stderr, "failed to compute default config file location");
  }
  { // userDataDir.default_
    char userDataDirStr[MAX_PATH];
    get_user_data_folder(userDataDirStr, MAX_PATH, PACKAGE_NAME);
    if(*userDataDirStr)
      userDataDir.default_ = std::filesystem::path(userDataDirStr);
    else
      fmt::println(stderr,
        "warning: failed to compute default user data directory location");
  }
  { // appDataDir.default_
    appDataDir.default_ = std::filesystem::path(INSTALL_FULL_APPDATADIR);

    #ifdef LINCITYNG_RELOCATABLE
    const std::filesystem::path invBin =
      std::filesystem::path().lexically_relative(INSTALL_BINDIR);
    const std::filesystem::path basePath(SDL_GetBasePath());
    const std::filesystem::path relocPrefix =
      (basePath / invBin).lexically_normal();
    if(!relocPrefix.empty()) {
      appDataDir.default_ = relocPrefix / INSTALL_APPDATADIR;
    }
    else {
      fmt::println(stderr,
        "error: failed to compute the relocation prefix: {}\n"
        "  Falling back to the install prefix.",
        SDL_GetError()
      );
    }
    #endif
  }
}

Config::~Config() {}

void Config::load(std::filesystem::path configFile) {
  if(configFile.empty())
    configFile = this->configFile.get();
  if(!std::filesystem::exists(configFile)) {
    fmt::println(stderr, "info: config file does not exist: {}", configFile);
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

        xmlpp::ustring xml_tag = xmlReader.get_name();
        xmlpp::ustring xml_val = xmlReader.read_inner_xml();
        if(xml_tag == "useOpenGL")
          useOpenGL.config = xmlParseConfig<bool>(xml_val);
        else if(xml_tag == "x")
          videoX.config = validateRange(xmlParseConfig<int>(xml_val), 640);
        else if(xml_tag == "y")
          videoY.config = validateRange(xmlParseConfig<int>(xml_val), 480);
        else if(xml_tag == "fullscreen")
          useFullScreen.config = xmlParseConfig<bool>(xml_val);
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

        xmlpp::ustring xml_tag = xmlReader.get_name();
        xmlpp::ustring xml_val = xmlReader.read_inner_xml();
        if(xml_tag == "soundVolume")
          soundVolume.config =
            validateRange(xmlParseConfig<int>(xml_val), 0, 100);
        else if(xml_tag == "musicVolume")
          musicVolume.config =
            validateRange(xmlParseConfig<int>(xml_val), 0, 100);
        else if(xml_tag == "soundEnabled")
          soundEnabled.config = xmlParseConfig<bool>(xml_val);
        else if(xml_tag == "musicEnabled")
          musicEnabled.config = xmlParseConfig<bool>(xml_val);
        else if(xml_tag == "musicTheme")
          musicTheme.config = xmlParseConfig<std::string>(xml_val);
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

        xmlpp::ustring xml_tag = xmlReader.get_name();
        xmlpp::ustring xml_val = xmlReader.read_inner_xml();
        if(xml_tag == "language")
          language.config = xmlParseConfig<std::string>(xml_val);
        else if(xml_tag == "WorldSideLen")
          worldSize.config =
            validateRange(xmlParseConfig<int>(xml_val), 50, 10000);
        else if(xml_tag == "carsEnabled")
          carsEnabled.config = xmlParseConfig<bool>(xml_val);
        else if(xml_tag == "appDataDir")
          appDataDir.config = xmlParseConfig<std::filesystem::path>(xml_val);
        else if(xml_tag == "userDataDir")
          userDataDir.config = xmlParseConfig<std::filesystem::path>(xml_val);
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
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"x", xmlFormatConfig<int>(videoX.config));
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"y", xmlFormatConfig<int>(videoY.config));
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"useOpenGL", xmlFormatConfig<bool>(useOpenGL.config));
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"fullscreen", xmlFormatConfig<bool>(useFullScreen.config));
    xmlTextWriterEndElement(xmlWriter);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"audio");
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"soundEnabled", xmlFormatConfig<bool>(soundEnabled.config));
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"soundVolume", xmlFormatConfig<int>(soundVolume.config));
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"musicEnabled", xmlFormatConfig<bool>(musicEnabled.config));
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"musicVolume", xmlFormatConfig<int>(musicVolume.config));
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"musicTheme", xmlFormatConfig<std::string>(musicTheme.config));
    xmlTextWriterEndElement(xmlWriter);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"game");
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"language", xmlFormatConfig<std::string>(language.config));
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"WorldSideLen", xmlFormatConfig<int>(worldSize.config));
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"carsEnabled", xmlFormatConfig<bool>(carsEnabled.config));
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"appDataDir", xmlFormatConfig<std::filesystem::path>(appDataDir.config));
      xmlTextWriterWriteElement(xmlWriter, (xmlStr)"userDataDir", xmlFormatConfig<std::filesystem::path>(userDataDir.config));
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
Config::init(int argc, char** argv) {
  if(const char *envConfigFile = getenv("LINCITYNG_CONFIG_FILE"))
    userDataDir.session = std::filesystem::path(envConfigFile);
  if(const char *envAppDataDir = getenv("LINCITYNG_APP_DATA_DIR"))
    appDataDir.session = std::filesystem::path(envAppDataDir);
  if(const char *envUserDataDir = getenv("LINCITYNG_USER_DATA_DIR"))
    userDataDir.session = std::filesystem::path(envUserDataDir);

  for(int argi = 1; argi < argc; ++argi) {
    std::string argStr = argv[argi];

    if(argStr == "-v" || argStr == "--version") {
      showVersion.session = true;
    } else if(argStr == "-h" || argStr == "--help") {
      showHelp.session = true;
    } else if(argStr == "--config" || argStr == "-c") {
      argi++;
      if(argi >= argc)
        throw std::runtime_error(fmt::format("{} needs a parameter", argStr));
      if(configFile.session)
        fmt::println(stderr, "warning: cannot load more than one config file.");
      configFile.session = std::filesystem::path(argv[argi]);
    } else if(argStr == "-g" || argStr == "--gl") {
      useOpenGL.session = true;
    } else if(argStr == "-s" || argStr == "--sdl") {
      useOpenGL.session = false;
    } else if(argStr == "-S" || argStr == "--size") {
      argi++;
      if(argi >= argc)
        throw std::runtime_error(fmt::format("{} needs a parameter", argStr));
      argStr = argv[argi];
      int x, y;
      if(sscanf(argStr.c_str(), "%ix%i", &x, &y) != 2
        || x <= 0 || y <= 0
      )
        throw std::runtime_error(
          fmt::format("failed to parse --size parameter: {}", argStr));
      videoX.session = x;
      videoY.session = y;
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
        throw std::runtime_error(fmt::format("{} needs a parameter", argStr));
      configFile.session = std::filesystem::path(argv[argi]);
    } else if(argStr == "--app-data") {
      argi++;
      if(argi >= argc)
        throw std::runtime_error(fmt::format("{} needs a parameter", argStr));
      appDataDir.session = std::filesystem::path(argv[argi]);
    } else if(argStr == "--user-data") {
      argi++;
      if(argi >= argc)
        throw std::runtime_error(fmt::format("{} needs a parameter", argStr));
      userDataDir.session = std::filesystem::path(argv[argi]);
    } else {
      throw std::runtime_error(
        fmt::format("unrecognized argument: {}", argStr));
    }
  }

  load(configFile.get());

#ifdef DEBUG
  std::cerr << "  config file: " << configFile.get()  << std::endl
            << " app data dir: " << appDataDir.get()  << std::endl
            << "user data dir: " << userDataDir.get() << std::endl;
#endif

  if(!std::filesystem::is_directory(appDataDir.get())) {
    fmt::println(stderr, "error: app data location is not a directory: {}"
      "\n  Use `--app-data` to set the correct app data location."
      " Otherwise, LinCity-NG will likely crash.",
      appDataDir.get()
    );
  }

  if(!std::filesystem::is_directory(userDataDir.get())) {
    fmt::println(stderr, "error: user data location is not a directory: {}"
      "\n  Use `--user-data` to set the correct app data location.",
      userDataDir.get()
    );
  }

#ifdef DISABLE_GL_MODE
  if(useOpenGL.get()) {
    useOpenGL.session = false;
    fmt::println(stderr,
      "warning: GL mode was requested, but it is disabled for this build."
      " Using SDL mode instead."
    );
  }
#endif
}

void
Config::printHelp(const std::string& command) {
  std::cout << PRETTY_NAME_VERSION << std::endl
    << std::endl
    << fmt::format("{} [-v|-h] [-w|-f] [-m] [-c <config>]", command)
    << std::endl << std::endl;

  struct Desc { std::string s; std::string l; std::string p; std::string d; };
  for(const Desc& o : (const Desc[]){
    {.s="-v",.l="--version",   .d=_("show version and exit")},
    {.s="-h",.l="--help",      .d=_("show this text and exit")},
    {.s="-g",.l="--gl",        .d=_("use OpenGL")},
    {.s="-s",.l="--sdl",       .d=_("use SDL")},
    {.s="-S",.l="--size",.p=_("<width>x<height>"),
      .d=_("specify screensize (eg. -S 1024x768)")},
    {.s="-w",.l="--window",    .d=_("run in window")},
    {.s="-f",.l="--fullscreen",.d=_("run fullscreen")},
    {.s="-m",.l="--mute",      .d=_("mute audio")},
    {.s="-c",.l="--config",.p=_("<file>"),.d=_("configuration file location")},
    {.l="--app-data",.p=_("<dir>"),.d=_("app data location")},
    {.l="--user-data",.p=_("<dir>"),.d=_("user data location")},
  }) {
    std::string line = fmt::format("{:2}", o.s);
    if(o.l != "") line += " " + o.l;
    if(o.p != "") line += " " + o.p;
    if(line.length() > 20) std::cout << line << std::endl, line = "";
    line = fmt::format("{:20} {}", line, o.d);
    std::cout << line << std::endl;
  }
}


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

template<typename V>
static std::optional<V>
xmlParseConfig(const xmlpp::ustring& s) {
  return s == "default" ? std::nullopt : std::optional(xmlParse<V>(s));
}

template<typename V>
static xmlStrF xmlFormatConfig(const std::optional<V>& option) {
  return option ? xmlFormat<V>(*option) : xmlStrF("default");
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

template class Config::Option<int>;
template class Config::Option<bool>;
template class Config::Option<std::string>;
template class Config::Option<std::filesystem::path>;

/** @file lincity-ng/Config.cpp */
