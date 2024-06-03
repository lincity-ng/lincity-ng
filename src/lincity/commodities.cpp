#include "commodities.hpp"

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
