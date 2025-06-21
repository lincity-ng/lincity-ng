/* ---------------------------------------------------------------------- *
 * src/lincity/messages.hpp
 * This file is part of Lincity-NG.
 *
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

#ifndef __LINCITYNG_LINCITY_MESSAGES_HPP__
#define __LINCITYNG_LINCITY_MESSAGES_HPP__

#include <exception>     // for exception_ptr, make_exception_ptr, current_e...
#include <memory>        // for static_pointer_cast, shared_ptr, make_shared
#include <string>        // for basic_string, string
#include <utility>       // for forward

#include "MapPoint.hpp"  // for MapPoint

class CannotBuildHereMessage;
class CannotBuildMessage;
class CannotBulldozeIncompleteMonumentMessage;
class CannotBulldozeNonemptyTipMessage;
class CannotBulldozeThisEverMessage;
class CannotBulldozeThisMessage;
class CannotEvacuateThisMessage;
class ConstructionGroup;
class DesertHereMessage;
class ExceptionMessage;
class FireStartedMessage;
class NoOreMessage;
class NoPeopleLeftMessage;
class NotEnoughStudentsMessage;
class NotEnoughTechMessage;
class NothingHereMessage;
class OutOfMoneyMessage;
class OutsideMapMessage;
class PortRequiresRiverMessage;
class RocketReadyMessage;
class RocketResultMessage;
class SpaceOccupiedMessage;
class SustainableEconomyMessage;

// This is solely to save the effort of const-qualifying dynamic casts.
template<typename R, typename T>
typename R::ptr dynamic_message_cast(const std::shared_ptr<const T>& t) {
  return std::dynamic_pointer_cast<const R>(t);
}
template<typename R, typename T>
typename R::ptr dynamic_message_cast(std::shared_ptr<const T>&& t) {
  return std::dynamic_pointer_cast<const R>(t);
}


class Message : public std::enable_shared_from_this<const Message> {
public:
  using ptr = std::shared_ptr<const Message>;

  virtual ~Message() = default;

  static ptr fromException(std::exception_ptr exception) {
    try { std::rethrow_exception(exception); }
    catch(const Message::Exception& ex) {
      return ex.getMessage();
    }
    catch(...) {
      // return ExceptionMessage::create(exception);
      return std::static_pointer_cast<const Message>(
        std::make_shared<const ExceptionMessage>(exception));
    }
  }
  static ptr fromCurrentException() {
    return fromException(std::current_exception());
  }

  /**
   * Returns a string briefly describing the message. This string is not meant
   * to be user-facing.
  **/
  virtual std::string str() const = 0;

  /**
   * Encapsulates this message in an exception.
  **/
  virtual std::exception_ptr exception() const = 0;

  [[noreturn]] void throwEx() const {
    std::rethrow_exception(exception());
  }

  class Exception : std::exception {
  public:
    virtual const char *what() const noexcept;

    ptr getMessage() const {
      return message;
    }

  protected:
    Exception(const ptr& message) :
      message(message), whatMsg(message->str())
    { }

    const ptr message;
    const std::string whatMsg;

    friend Message;
  };

protected:
  Message() = default;
};


class ExceptionMessage : public Message {
public:
  using ptr = std::shared_ptr<const ExceptionMessage>;

  ExceptionMessage(std::exception_ptr exception) : exception_(exception) { }

  static ptr create(std::exception_ptr exception) {
    return std::make_shared<const ExceptionMessage>(exception);
  }
  static ptr createCurrent() {
    return create(std::current_exception());
  }

  virtual std::string str() const override;
  virtual std::exception_ptr exception() const override { return exception_; }

  std::exception_ptr getException() const { return exception_; }

protected:
  std::exception_ptr exception_;
};


#define DEFINE_MESSAGE_BOILERPLATE(MESSAGE_TYPE, BASE_MESSAGE_TYPE) \
public: \
  using ptr = std::shared_ptr<const MESSAGE_TYPE>; \
  class Exception; \
  template<typename... A> \
  static ptr create(A&&... args) { \
    return std::make_shared<const MESSAGE_TYPE>(std::forward<A>(args)...); \
  } \
  virtual std::exception_ptr exception() const override { \
    return std::make_exception_ptr(Exception( \
      std::static_pointer_cast<const MESSAGE_TYPE>( \
        shared_from_this()))); \
  } \
  class Exception : public BASE_MESSAGE_TYPE::Exception { \
  public: \
    ptr getMessage() const { \
      return std::static_pointer_cast<const MESSAGE_TYPE>(message); \
    } \
  protected: \
    Exception(const ptr& message) : \
      BASE_MESSAGE_TYPE::Exception(message) { } \
    friend MESSAGE_TYPE; \
  }


class CannotBuildMessage : public Message {
public:
  CannotBuildMessage(
    ConstructionGroup& grp,
    Message::ptr reason = nullptr
  ) : group(grp), reason(reason) { }

  virtual std::string str() const override;

  ConstructionGroup& getGroup() const { return group; }
  Message::ptr getReason() const { return reason; }

protected:
  ConstructionGroup& group;
  const Message::ptr reason;

  DEFINE_MESSAGE_BOILERPLATE(CannotBuildMessage, Message);
};


class CannotBuildHereMessage : public CannotBuildMessage {
public:
  CannotBuildHereMessage(
    ConstructionGroup& grp,
    const MapPoint point,
    Message::ptr reason = nullptr
  ) : CannotBuildMessage(grp, reason), point(point) { }

  virtual std::string str() const override;

  MapPoint getPoint() const { return point; }

protected:
  const MapPoint point;

  DEFINE_MESSAGE_BOILERPLATE(CannotBuildHereMessage, CannotBuildMessage);
};


class CannotBulldozeThisMessage : public Message {
public:
  CannotBulldozeThisMessage(const MapPoint point, ConstructionGroup &grp) :
    point(point), group(grp) { }

  virtual std::string str() const override;

  MapPoint getPoint() const { return point; }
  ConstructionGroup& getGroup() const { return group; }

protected:
  const MapPoint point;
  ConstructionGroup& group;

  DEFINE_MESSAGE_BOILERPLATE(CannotBulldozeThisMessage, Message);
};


class CannotEvacuateThisMessage : public Message {
public:
  CannotEvacuateThisMessage(const MapPoint point, ConstructionGroup &grp) :
    point(point), group(grp) { }

  virtual std::string str() const override;

  MapPoint getPoint() const { return point; }
  ConstructionGroup& getGroup() const { return group; }

protected:
  const MapPoint point;
  ConstructionGroup& group;

  DEFINE_MESSAGE_BOILERPLATE(CannotEvacuateThisMessage, Message);
};


class FireStartedMessage : public Message {
public:
  FireStartedMessage(MapPoint point, const ConstructionGroup& grp) :
    point(point), group(grp) { }

  virtual std::string str() const override;

  MapPoint getPoint() const { return point; }
  const ConstructionGroup& getGroup() const { return group; }

protected:
  MapPoint point;
  const ConstructionGroup& group;

  DEFINE_MESSAGE_BOILERPLATE(FireStartedMessage, Message);
};


class OutOfMoneyMessage : public Message {
public:
  OutOfMoneyMessage(bool outOfCredit) : outOfCredit(outOfCredit) { }

  virtual std::string str() const override {
    return outOfCredit ? "no more credit" : "out of money";
  }

  bool isOutOfCredit() const { return outOfCredit; }

protected:
  bool outOfCredit;

  DEFINE_MESSAGE_BOILERPLATE(OutOfMoneyMessage, Message);
};


class NoOreMessage : public Message {
public:
  NoOreMessage() { }

  virtual std::string str() const override {
    return "no ore left";
  }

  DEFINE_MESSAGE_BOILERPLATE(NoOreMessage, Message);
};


class NothingHereMessage : public Message {
public:
  NothingHereMessage(MapPoint point) : point(point) { }

  virtual std::string str() const override;

  const MapPoint getPoint() { return point; }

protected:
  const MapPoint point;

  DEFINE_MESSAGE_BOILERPLATE(NothingHereMessage, Message);
};


class DesertHereMessage : public Message {
public:
  DesertHereMessage(MapPoint point) : point(point) { }

  virtual std::string str() const override;

  const MapPoint getPoint() { return point; }

protected:
  const MapPoint point;

  DEFINE_MESSAGE_BOILERPLATE(DesertHereMessage, Message);
};


class PortRequiresRiverMessage : public Message {
public:
  PortRequiresRiverMessage() { }

  virtual std::string str() const override {
    return "ports require connection to river along the east side";
  }

  DEFINE_MESSAGE_BOILERPLATE(PortRequiresRiverMessage, Message);
};


class NotEnoughStudentsMessage : public Message {
public:
  NotEnoughStudentsMessage() { }

  virtual std::string str() const override {
    return "not enough students";
  }

  DEFINE_MESSAGE_BOILERPLATE(NotEnoughStudentsMessage, Message);
};


class OutsideMapMessage : public Message {
public:
  OutsideMapMessage(const MapPoint point) { }

  virtual std::string str() const override;

  MapPoint getPoint() { return point; }

protected:
  const MapPoint point;

  DEFINE_MESSAGE_BOILERPLATE(OutsideMapMessage, Message);
};


class NotEnoughTechMessage : public Message {
public:
  NotEnoughTechMessage(int currentTech, int requiredTech) :
    currentTech(currentTech), requiredTech(requiredTech)
  { }

  virtual std::string str() const override;

  int getCurrentTech() const { return currentTech; }
  int getRequiredTech() const { return requiredTech; }

protected:
  const int currentTech;
  const int requiredTech;

  DEFINE_MESSAGE_BOILERPLATE(NotEnoughTechMessage, Message);
};


class SpaceOccupiedMessage : public Message {
public:
  SpaceOccupiedMessage(const MapPoint point) : point(point) { }

  virtual std::string str() const override;

  MapPoint getPoint() { return point; }

protected:
  const MapPoint point;

  DEFINE_MESSAGE_BOILERPLATE(SpaceOccupiedMessage, Message);
};


class SustainableEconomyMessage : public Message {
public:
  SustainableEconomyMessage(int time) : time(time) { }

  virtual std::string str() const override;

  int getTime() { return time; }

protected:
  const int time;

  DEFINE_MESSAGE_BOILERPLATE(SustainableEconomyMessage, Message);
};


class NoPeopleLeftMessage : public Message {
public:
  NoPeopleLeftMessage(int time) : time(time) { }

  virtual std::string str() const override;

  int getTime() { return time; }

protected:
  const int time;

  DEFINE_MESSAGE_BOILERPLATE(NoPeopleLeftMessage, Message);
};


class CannotBulldozeIncompleteMonumentMessage :
  public CannotBulldozeThisMessage
{
public:
  CannotBulldozeIncompleteMonumentMessage(const MapPoint point);

  virtual std::string str() const override;

  DEFINE_MESSAGE_BOILERPLATE(CannotBulldozeIncompleteMonumentMessage,
    CannotBulldozeThisMessage);
};


class CannotBulldozeNonemptyTipMessage : public CannotBulldozeThisMessage {
public:
  CannotBulldozeNonemptyTipMessage(const MapPoint point);

  virtual std::string str() const override;

  DEFINE_MESSAGE_BOILERPLATE(CannotBulldozeNonemptyTipMessage,
    CannotBulldozeThisMessage);
};


class CannotBulldozeThisEverMessage : public CannotBulldozeThisMessage {
public:
  CannotBulldozeThisEverMessage(const MapPoint point, ConstructionGroup &grp)
    : CannotBulldozeThisMessage(point, grp)
  { }

  virtual std::string str() const override;

  DEFINE_MESSAGE_BOILERPLATE(CannotBulldozeThisEverMessage,
    CannotBulldozeThisMessage);
};


class RocketReadyMessage : public Message {
public:
  RocketReadyMessage(const MapPoint point) : point(point) { }

  virtual std::string str() const override;

  MapPoint getPoint() const { return point; }

protected:
  const MapPoint point;

  DEFINE_MESSAGE_BOILERPLATE(RocketReadyMessage, Message);
};


class RocketResultMessage : public Message {
public:
  enum class LaunchResult { FAIL, SUCCESS, EVAC, EVAC_WIN };

  RocketResultMessage(const MapPoint point, LaunchResult result)
    : point(point), result(result)
  { }

  virtual std::string str() const override;

  MapPoint getPoint() const { return point; }
  LaunchResult getResult() const { return result; }

protected:
  const MapPoint point;
  const LaunchResult result;

  DEFINE_MESSAGE_BOILERPLATE(RocketResultMessage, Message);
};


#endif // __LINCITYNG_LINCITY_MESSAGES_HPP__
