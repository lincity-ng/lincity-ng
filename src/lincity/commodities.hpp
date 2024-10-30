#ifndef __LINCITY_COMMODITIES_HPP__
#define __LINCITY_COMMODITIES_HPP__

enum Commodity : int {
  STUFF_INIT = 0,
  STUFF_FOOD = STUFF_INIT,
  STUFF_LABOR,
  STUFF_COAL,
  STUFF_GOODS,
  STUFF_ORE,
  STUFF_STEEL,
  STUFF_WASTE,
  STUFF_LOVOLT,
  STUFF_HIVOLT,
  STUFF_WATER,
  STUFF_COUNT
};
Commodity& operator++(Commodity& stuff);
Commodity operator++(Commodity& stuff, int);

extern const char *commodityNames[]; // deprecated: use commodityName

extern const char *commodityName(Commodity stuff); // localized for UI
extern const char *commodityStandardName(Commodity stuff); // standardized for load/save
/**
 * Used in load/save logic to get the commodity from the standard name or a past
 * standard name. Returns `STUFF_COUNT` if the name is not recognized.
**/
extern Commodity commodityFromStandardName(const char *name);

struct CommodityRule {
  int maxload;
  bool take;
  bool give;
};

#endif // __LINCITY_COMMODITIES_HPP__
