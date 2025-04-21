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
#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <filesystem>  // for path
#include <string>      // for basic_string, string

class Config
{
public:
  Config();
  ~Config();

  std::filesystem::path configFile;
  std::filesystem::path appDataDir;
  std::filesystem::path userDataDir;
  bool appDataDirIsDefault;
  bool userDataDirIsDefault;

  bool useOpenGL;
  bool useFullScreen;
  bool restartOnChangeScreen;
  int videoX, videoY;
  int monthgraphW, monthgraphH;

  // sound volume 0..100 (0=silent)
  int soundVolume;
  // music volume 0..100
  int musicVolume;
  bool soundEnabled;
  bool musicEnabled;
  bool carsEnabled;

  std::string language;
  std::string musicTheme;

  void load(std::filesystem::path configPath = std::filesystem::path());
  void save(std::filesystem::path configPath = std::filesystem::path());

  void parseCommandLine(int argc, char** argv);
};

Config* getConfig();

#endif

/** @file lincity-ng/Config.hpp */
