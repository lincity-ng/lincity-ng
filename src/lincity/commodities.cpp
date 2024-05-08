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
  N_("Jobs"),
  N_("Coal"),
  N_("Goods"),
  N_("Ore"),
  N_("Steel"),
  N_("Waste"),
  N_("kWh"),
  N_("MWh"),
  N_("Water"),
  "Unknown",
  "Unknown",
  "Unknown",
};
