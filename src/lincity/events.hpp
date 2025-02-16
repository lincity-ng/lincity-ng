/* ---------------------------------------------------------------------- *
 * src/lincity/events.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2025 David Bears <dbear4q@gmail.com>
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

#ifndef __LINCITYNG_LINCITY_EVENTS_HPP__
#define __LINCITYNG_LINCITY_EVENTS_HPP__

class World;
class MapTile;

class LincityEvent {
public:
  LincityEvent(World& world);

  World& getWorld() const;

private:
  World& world;
};

class MessageEvent : public LincityEvent {
public:
  enum class Level : int {
    FATAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG_
  };

  MessageEvent(World& world, std::string message, Level level = Level::INFO);

  const std::string& getMessage() const;
  Level getLevel() const;

private:
  const std::string message;
  const Level level;
};

class FireStartedMessage : public MessageEvent {
public:
  FireStartedMessage(World& world, MapTile& tile);
};

class CannotBuildOnCreditMessage : public MessageEvent {
public:
  CannotBuildOnCreditMessage(World& world);
};

class OutOfMoneyMessage : public MessageEvent {
public:
  OutOfMoneyMessage(World& world);
};

class UpdateEvent : public LincityEvent {
public:
  enum class Type {
    POPULATION,
    TECH,
    MONEY,
    FOOD,
    LABOR,
    GOODS,
    COAL,
    ORE,
    STEEL,
    POLLUTION,
    LOVOLT,
    HIVOLT,
    WATER,
    WASTE,
    TIME,
    MAP,
    SUSTAINABILITY,
  };

  UpdateEvent(World& world, Type type);

  Type getType() const;

private:
  const Type type;
};

#endif // __LINCITYNG_LINCITY_EVENTS_HPP__
