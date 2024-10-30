#include "commodities.hpp"

#include <cstring>

#include "tinygettext/gettext.hpp"  // for N_

Commodity& operator++(Commodity& stuff) {
  return stuff = (Commodity)((int)stuff + 1);
}
Commodity operator++(Commodity& stuff, int) {
  Commodity tmp(stuff);
  ++stuff;
  return tmp;
}

const char *commodityNames[] = {
  N_("Food"),
  N_("Labor"),
  N_("Coal"),
  N_("Goods"),
  N_("Ore"),
  N_("Steel"),
  N_("Waste"),
  N_("Lo-Volt"),
  N_("Hi-Volt"),
  N_("Water"),
  "Unknown",
  "Unknown",
  "Unknown",
};
const char *commodityStandardNames[] = {
  "food",
  "labor",
  "coal",
  "goods",
  "ore",
  "steel",
  "waste",
  "lovolt",
  "hivolt",
  "water"
};

const char *commodityName(Commodity stuff) {
  if(stuff >= STUFF_INIT && stuff < STUFF_COUNT)
    return commodityNames[stuff];
  else
    return "Unknown";
}

const char *commodityStandardName(Commodity stuff){
  if(stuff >= STUFF_INIT && stuff < STUFF_COUNT)
    return commodityStandardNames[stuff];
  else
    return "unknown";
}

Commodity commodityFromStandardName(const char *name) {
  if     (!std::strcmp(name, "food"))   return STUFF_FOOD;
  else if(!std::strcmp(name, "labor"))  return STUFF_LABOR;
  else if(!std::strcmp(name, "coal"))   return STUFF_COAL;
  else if(!std::strcmp(name, "goods"))  return STUFF_GOODS;
  else if(!std::strcmp(name, "ore"))    return STUFF_ORE;
  else if(!std::strcmp(name, "steel"))  return STUFF_STEEL;
  else if(!std::strcmp(name, "waste"))  return STUFF_WASTE;
  else if(!std::strcmp(name, "lovolt")) return STUFF_LOVOLT;
  else if(!std::strcmp(name, "hivolt")) return STUFF_HIVOLT;
  else if(!std::strcmp(name, "water"))  return STUFF_WATER;
  else                                  return STUFF_COUNT;
}
