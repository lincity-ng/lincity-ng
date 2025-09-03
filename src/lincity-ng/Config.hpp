/* ---------------------------------------------------------------------- *
 * src/lincity-ng/Config.hpp
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

#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <filesystem>  // for path
#include <optional>
#include <string>      // for basic_string, string

class Config
{
public:
  template<typename T>
  class Option {
  public:
    std::optional<T> default_;
    std::optional<T> config = std::nullopt;
    std::optional<T> session = std::nullopt;

    const T& get() const;
    bool isDefault() const;

    void sessionToConfig();

  private:
    Option();
    Option(const T& default_);

    friend Config;
  };

  Config();
  ~Config();

  Option<std::filesystem::path> configFile;
  Option<std::filesystem::path> appDataDir;
  Option<std::filesystem::path> userDataDir;

  Option<bool> useOpenGL;
  Option<bool> useFullScreen;
  Option<int> videoX, videoY;
  Option<bool> showHelp;
  Option<bool> showVersion;

  // sound volume 0..100 (0=silent)
  Option<int> soundVolume;
  // music volume 0..100
  Option<int> musicVolume;
  Option<bool> soundEnabled;
  Option<bool> musicEnabled;
  Option<bool> carsEnabled;

  Option<std::string> language;
  Option<std::string> musicTheme;

  Option<int> worldSize;

  void load(std::filesystem::path configPath = std::filesystem::path());
  void save(std::filesystem::path configPath = std::filesystem::path());

  void init(int argc, char** argv);

  static void printHelp(const std::string& command);
};

extern template class Config::Option<int>;
extern template class Config::Option<bool>;
extern template class Config::Option<std::string>;
extern template class Config::Option<std::filesystem::path>;

Config* getConfig();

#endif

/** @file lincity-ng/Conextern fig.hpp */
