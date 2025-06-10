/* ---------------------------------------------------------------------- *
 * src/lincity/xmlloadsave.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2024-2025 David Bears <dbear4q@gmail.com>
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

#include "xmlloadsave.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/parser.h>                // for XML_PARSE_NONET
#include <libxml/xmlIO.h>                 // for xmlOutputBufferClose, xmlOu...
#include <libxml/xmlerror.h>              // for XML_ERR_OK
#include <libxml/xmlreader.h>             // for xmlReaderForIO, xmlTextRead...
#include <libxml/xmlversion.h>            // for LIBXML_VERSION
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteFormatEle...
#include <zlib.h>                         // for gzclose, gzFile, gzopen
#include <algorithm>                      // for max, min
#include <array>                          // for array
#include <cassert>                        // for assert
#include <cctype>                         // for isspace
#include <cstring>                        // for NULL, size_t
#include <deque>                          // for deque
#include <filesystem>                     // for path
#include <functional>                     // for function
#include <iostream>                       // for operator<<, basic_ostream
#include <list>                           // for list, _List_iterator
#include <memory>                         // for unique_ptr, shared_ptr
#include <regex>                          // for match_results, operator==
#include <set>                            // for _Rb_tree_const_iterator, set
#include <sstream>                        // for basic_stringstream, basic_o...
#include <stdexcept>                      // for runtime_error, invalid_argu...
#include <string>                         // for basic_string, operator==, stoi
#include <unordered_map>                  // for unordered_map, operator!=
#include <utility>                        // for pair
#include <vector>                         // for vector

#include "MapPoint.hpp"                   // for MapPoint
#include "commodities.hpp"                // for Commodity, CommodityRule
#include "lin-city.hpp"                     // for VOLATILE_FLAGS, FLAG_CRICKE...
#include "lintypes.hpp"                     // for xmlTextWriterPtr, Construction
#include "stats.hpp"                        // for Stats, Stat
#include "world.hpp"                        // for World, MapTile, Map, Ground
#include "util.hpp"

static void saveGlobals(xmlTextWriterPtr xmlWriter, const World& world);
static void loadGlobals(xmlpp::TextReader& xmlReader, World& World,
  unsigned int ldsv_version
);
static void loadGlobals_v2130(xmlpp::TextReader& xmlReader, World& world,
  unsigned int ldsv_version
);
static void saveMap(xmlTextWriterPtr xmlWriter, const Map& map);
static void loadMap(xmlpp::TextReader& xmlReader, World& world,
  unsigned int ldsv_version
);
static void saveMapTile(xmlTextWriterPtr xmlWriter, const MapTile& tile);
static void loadMapTile(xmlpp::TextReader& xmlReader, MapTile& tile,
  unsigned int ldsv_version
);
template<typename A>
static void writeArray(xmlTextWriterPtr xmlWriter, const A& array,
  std::function<void(xmlTextWriterPtr, const typename A::value_type&)>
    writeElement
);
template<typename A>
static void readArray(xmlpp::TextReader& xmlReader, A& array,
  std::function<void(xmlpp::TextReader&, typename A::value_type&)> readElement
);
static void readArray_old(xmlpp::TextReader& xmlReader, std::deque<int>& array);
static void readPbar_old(xmlpp::TextReader& xmlReader, std::deque<int>& array);
static void readPbar_old(xmlpp::TextReader& xmlReader,
  std::deque<Stats::Inventory<>>& array
);

void
World::save(const std::filesystem::path& filename) const {
  std::string gz_name;
  gzFile gz_file = gzopen(filename.string().c_str(), "wb");
  if(!gz_file)
    throw std::runtime_error(
      std::string("failed to open file: ") + filename.string());

  int xmlStatus = XML_ERR_OK;
  {
  xmlOutputBufferPtr xmlWriterBuffer = xmlOutputBufferCreateIO(
    [](void *ctx, const char *buf, int len){
      return gzwrite((gzFile)ctx, buf, len);},
    [](void *ctx){return gzclose((gzFile)ctx);},
    gz_file, NULL);
  if(!xmlWriterBuffer) {
    gzclose(gz_file);
    throw std::runtime_error("failed to create XML output buffer");
  }
  xmlTextWriterPtr xmlWriter = xmlNewTextWriter(xmlWriterBuffer);
  if(!xmlWriter) {
    xmlOutputBufferClose(xmlWriterBuffer);
    throw std::runtime_error("failed to create XML text writer");
  }
  std::shared_ptr<xmlTextWriter> xmlWriterCloser(xmlWriter,
#if LIBXML_VERSION >= 21300
    [&xmlStatus](xmlTextWriterPtr xmlWriter) {
      xmlStatus = xmlTextWriterClose(xmlWriter);
      xmlFreeTextWriter(xmlWriter);
    }
#else
    [&xmlStatus, &xmlWriterBuffer](xmlTextWriterPtr xmlWriter) {
      xmlStatus = xmlOutputBufferClose(xmlWriterBuffer);
      if(xmlStatus < 0) xmlStatus = -xmlStatus;
      else xmlStatus = XML_ERR_OK;
      xmlFreeTextWriter(xmlWriter);
    }
#endif
  );

#ifdef DEBUG
  xmlTextWriterSetIndent(xmlWriter, true);
  xmlTextWriterSetIndentString(xmlWriter, (xmlStr)"  ");
#else
  xmlTextWriterSetIndent(xmlWriter, false);
#endif

  xmlTextWriterStartDocument(xmlWriter, NULL, NULL, NULL);
  xmlTextWriterWriteComment(xmlWriter,
    (xmlStr)"This file is a lincity savegame.");
  xmlTextWriterStartElement(xmlWriter, (xmlStr)"lc-game");
    xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"ldsv-version", "%u",
      LOADSAVE_VERSION_CURRENT);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"globals");
      saveGlobals(xmlWriter, *this);
    xmlTextWriterEndElement(xmlWriter);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"map");
      saveMap(xmlWriter, map);
    xmlTextWriterEndElement(xmlWriter);
  xmlTextWriterEndElement(xmlWriter);
  xmlTextWriterEndDocument(xmlWriter);
  } // end scope closed xmlWriter and sets xmlStatus
  if(xmlStatus) {
    throw std::runtime_error(
      std::string("XML parser error: ") + std::to_string(xmlStatus));
  }
}

std::unique_ptr<World>
World::load(const std::filesystem::path& filename) {
  gzFile gz_file = gzopen(filename.string().c_str(), "rb");
  if(!gz_file)
    throw std::runtime_error(
      std::string("failed to open file: ") + filename.string());

  xmlTextReaderPtr xmlCReader = xmlReaderForIO(
    [](void *ctx, char *buf, int len){
      return gzread((gzFile)ctx, buf, len);},
    [](void *ctx){return gzclose((gzFile)ctx);},
    gz_file,
    NULL,
    NULL,
    XML_PARSE_NONET
  );
  if(!xmlCReader) {
    gzclose(gz_file);
    throw std::runtime_error("failed to initialize XML parser");
  }

  xmlpp::TextReader xmlReader(xmlCReader);

  // initialize the reader and seek to the first node
  if(!xmlReader.read())
    throw std::runtime_error("save file is empty");

  // find the SaveGame node
  while(true) {
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element);
    else if(xmlReader.get_name() == "lc-game")
      break;
    else if(xmlReader.get_name() == "SaveGame") {
      // detected pre-2.13.0 format
      throw std::runtime_error(
        std::string("Detected a load/save format that is too old.") +
        " See the readme for how to update the load/save format.");
    }
    else
      unexpectedXmlElement(xmlReader);
    if(!xmlReader.next())
      throw std::runtime_error("failed to find <lc-game> element in save file");
  }

  // parse load/save version
  std::string versionStr = xmlReader.get_attribute("ldsv-version");
  if(versionStr.empty())
    throw std::runtime_error("failed to parse load/save version");
  unsigned int ldsv_version = std::stoi(versionStr);
  if(ldsv_version > LOADSAVE_VERSION_CURRENT)
    throw std::runtime_error("load/save version too new");
  else if(ldsv_version < LOADSAVE_VERSION_COMPAT)
    throw std::runtime_error("load/save version too old");

  std::unique_ptr<World> world(new World());

  // parse sections
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "lc-game");
  [[used_in_assert]] int depth = xmlReader.get_depth();
  assert(depth == 0);
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    if(xmlReader.get_name() == "globals") {
      loadGlobals(xmlReader, *world, ldsv_version);
    }
    else if(xmlReader.get_name() == "map") {
      loadMap(xmlReader, *world, ldsv_version);
    }
    else {
      unexpectedXmlElement(xmlReader);
    }
    xmlReader.next();
  }
  assert(xmlReader.get_name() == "lc-game");
  assert(xmlReader.get_depth() == depth);

  // warn about other elements in the file
  while(xmlReader.next()) {
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element)
      continue;
    unexpectedXmlElement(xmlReader);
  }

  return world;
}

void unexpectedXmlElement(xmlpp::TextReader& xmlReader) {
  std::cerr << "warning: skipping unexpected element <"
    << xmlReader.get_name() << ">" << std::endl;
}

void missingXmlElement(xmlpp::TextReader& xmlReader, const std::string& name) {
  throw std::runtime_error((std::stringstream() <<
    "missing XML element <" << name << "> in <" << xmlReader.get_name() << ">"
  ).str());
}

static void saveGlobals(xmlTextWriterPtr xmlWriter, const World& world) {

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"given_scene",                 "%s", world.given_scene.c_str());
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_time",                  "%d", world.total_time);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"people_pool",                 "%d", world.people_pool);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_money",                 "%d", world.total_money);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tech_level",                  "%d", world.tech_level);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"rockets_launched",            "%d", world.rockets_launched);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"rockets_launched_success",    "%d", world.rockets_launched_success);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal_survey_done",            "%d", world.coal_survey_done);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"game_end",                    "%d", world.gameEnd);

  xmlTextWriterStartElement(xmlWriter, (xmlStr)"money_rates");
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"income_tax_rate",           "%d", world.money_rates.income_tax);
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal_tax_rate",             "%d", world.money_rates.coal_tax);
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"dole_rate",                 "%d", world.money_rates.dole);
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"transport_cost_rate",       "%d", world.money_rates.transport_cost);
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"goods_tax_rate",            "%d", world.money_rates.goods_tax);
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"export_tax_rate",           "%d", world.money_rates.export_tax);
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"import_cost_rate",          "%d", world.money_rates.import_cost);
  xmlTextWriterEndElement(xmlWriter);

  xmlTextWriterStartElement(xmlWriter, (xmlStr)"taxable");
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"labor",                     "%d", world.taxable.labor);
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal",                      "%d", world.taxable.coal);
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"goods",                     "%d", world.taxable.goods);
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"trade_ex",                  "%d", world.taxable.trade_ex);
  xmlTextWriterEndElement(xmlWriter);

  xmlTextWriterStartElement(xmlWriter, (xmlStr)"tradeRules");
    for(Commodity c = STUFF_INIT; c < STUFF_COUNT; c++) {
      const char * const &cname = commodityStandardName(c);
      xmlTextWriterWriteFormatElement(xmlWriter,
        (xmlStr)(std::string("import_")+cname+"_enable").c_str(), "%d",
        world.tradeRule[c].take);
      xmlTextWriterWriteFormatElement(xmlWriter,
        (xmlStr)(std::string("export_")+cname+"_enable").c_str(), "%d",
        world.tradeRule[c].give);
    }
  xmlTextWriterEndElement(xmlWriter);

  xmlTextWriterStartElement(xmlWriter, (xmlStr)"stats");
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"highest_tech_level",        "%d", world.stats.highest_tech_level);
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_pollution",           "%d", world.stats.total_pollution);

    xmlTextWriterStartElement(xmlWriter, (xmlStr)"population");
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"population_lm",           "%d", world.stats.population.population_m.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"population_mtd",          "%d", world.stats.population.population_m.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"housed_lm",               "%d", world.stats.population.housed_m.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"housed_mtd",              "%d", world.stats.population.housed_m.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"starving_lm",             "%d", world.stats.population.starving_m.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"starving_mtd",            "%d", world.stats.population.starving_m.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unemployed_lm",           "%d", world.stats.population.unemployed_m.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unemployed_mtd",          "%d", world.stats.population.unemployed_m.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"housing_lm",              "%d", world.stats.population.housing_m.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"housing_mtd",             "%d", world.stats.population.housing_m.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"births_lm",               "%d", world.stats.population.births_m.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"births_mtd",              "%d", world.stats.population.births_m.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"deaths_lm",               "%d", world.stats.population.deaths_m.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"deaths_mtd",              "%d", world.stats.population.deaths_m.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unnat_deaths_lm",         "%d", world.stats.population.unnat_deaths_m.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unnat_deaths_mtd",        "%d", world.stats.population.unnat_deaths_m.acc);

      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"evacuated_total",         "%d", world.stats.population.evacuated_t);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"births_total",            "%d", world.stats.population.births_t);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"pollution_deaths_total",  "%d", world.stats.population.pollution_deaths_t);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"starve_deaths_total",     "%d", world.stats.population.starve_deaths_t);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unemployed_days_total",   "%d", world.stats.population.unemployed_days_t);

      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unemployed_history",      "%f", world.stats.population.unemployed_history);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"pollution_deaths_history", "%f", world.stats.population.pollution_deaths_history);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"starve_deaths_history",   "%f", world.stats.population.starve_deaths_history);

      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"max_pop_ever",            "%d", world.stats.population.max_pop_ever);
    xmlTextWriterEndElement(xmlWriter);

    xmlTextWriterStartElement(xmlWriter, (xmlStr)"income");
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"income_tax_ly",           "%d", world.stats.income.income_tax.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"income_tax_ytd",          "%d", world.stats.income.income_tax.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal_tax_ly",             "%d", world.stats.income.coal_tax.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal_tax_ytd",            "%d", world.stats.income.coal_tax.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"goods_tax_ly",            "%d", world.stats.income.goods_tax.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"goods_tax_ytd",           "%d", world.stats.income.goods_tax.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"export_tax_ly",           "%d", world.stats.income.export_tax.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"export_tax_ytd",          "%d", world.stats.income.export_tax.acc);
    xmlTextWriterEndElement(xmlWriter);

    xmlTextWriterStartElement(xmlWriter, (xmlStr)"expenses");
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"construction_ly",         "%d", world.stats.expenses.construction.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"construction_ytd",        "%d", world.stats.expenses.construction.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coalSurvey_ly",           "%d", world.stats.expenses.coalSurvey.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coalSurvey_ytd",          "%d", world.stats.expenses.coalSurvey.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"import_ly",               "%d", world.stats.expenses.import.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"import_ytd",              "%d", world.stats.expenses.import.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unemployment_ly",         "%d", world.stats.expenses.unemployment.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unemployment_ytd",        "%d", world.stats.expenses.unemployment.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"transport_ly",            "%d", world.stats.expenses.transport.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"transport_ytd",           "%d", world.stats.expenses.transport.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"windmill_ly",             "%d", world.stats.expenses.windmill.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"windmill_ytd",            "%d", world.stats.expenses.windmill.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"university_ly",      "%d", world.stats.expenses.university.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"university_ytd",     "%d", world.stats.expenses.university.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"recycle_ly",              "%d", world.stats.expenses.recycle.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"recycle_ytd",             "%d", world.stats.expenses.recycle.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"deaths_ly",               "%d", world.stats.expenses.deaths.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"deaths_ytd",              "%d", world.stats.expenses.deaths.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"health_ly",               "%d", world.stats.expenses.health.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"health_ytd",              "%d", world.stats.expenses.health.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"rockets_ly",              "%d", world.stats.expenses.rockets.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"rockets_ytd",             "%d", world.stats.expenses.rockets.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"school_ly",               "%d", world.stats.expenses.school.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"school_ytd",              "%d", world.stats.expenses.school.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"firestation_ly",          "%d", world.stats.expenses.firestation.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"firestation_ytd",         "%d", world.stats.expenses.firestation.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"cricket_ly",              "%d", world.stats.expenses.cricket.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"cricket_ytd",             "%d", world.stats.expenses.cricket.acc);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"interest_ly",             "%d", world.stats.expenses.interest.stat);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"interest_ytd",            "%d", world.stats.expenses.interest.acc);
    xmlTextWriterEndElement(xmlWriter);

    xmlTextWriterStartElement(xmlWriter, (xmlStr)"sustainability");
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"mining_flag",             "%d", world.stats.sustainability.mining_flag);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"mining_years",            "%d", world.stats.sustainability.mining_years);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"trade_flag",              "%d", world.stats.sustainability.trade_flag);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"trade_years",             "%d", world.stats.sustainability.trade_years);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"old_money",               "%d", world.stats.sustainability.old_money);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"money_years",             "%d", world.stats.sustainability.money_years);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"old_population",          "%d", world.stats.sustainability.old_population);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"population_years",        "%d", world.stats.sustainability.population_years);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"old_tech",                "%d", world.stats.sustainability.old_tech);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tech_years",              "%d", world.stats.sustainability.tech_years);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"fire_years",              "%d", world.stats.sustainability.fire_years);
      xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sustainable",             "%d", world.stats.sustainability.sustainable);
    xmlTextWriterEndElement(xmlWriter);

    xmlTextWriterStartElement(xmlWriter, (xmlStr)"history");
      std::vector<std::pair<std::deque<int>, xmlStr>> histories_int({
        {world.stats.history.pop, (xmlStr)"pop"},
        {world.stats.history.starve, (xmlStr)"starve"},
        {world.stats.history.nojobs, (xmlStr)"nojobs"},
        {world.stats.history.ppool, (xmlStr)"ppool"},
        {world.stats.history.tech, (xmlStr)"tech"},
        {world.stats.history.money, (xmlStr)"money"},
        {world.stats.history.pollution, (xmlStr)"pollution"},
        {world.stats.history.births, (xmlStr)"births"},
        {world.stats.history.deaths, (xmlStr)"deaths"},
        {world.stats.history.unnat_deaths, (xmlStr)"unnat_deaths"}
      });
      for(auto& history : histories_int) {
        xmlTextWriterStartElement(xmlWriter, history.second);
          writeArray(xmlWriter, history.first,
            [](xmlTextWriterPtr xmlWriter, const int& el) {
              xmlTextWriterWriteFormatString(xmlWriter, "%d", el);
            }
          );
        xmlTextWriterEndElement(xmlWriter);
      }

      std::vector<std::pair<
        std::deque<Stats::Inventory<int>>,
        xmlStr
      >> histories_inv({
        {world.stats.history.tenants, (xmlStr)"tenants"}
      });
      for(Commodity s = STUFF_INIT; s < STUFF_COUNT; s++) {
        histories_inv.push_back({
          world.stats.history.inventory[s],
          (xmlStr)commodityStandardName(s)
        });
      }
      for(auto& history : histories_inv) {
        xmlTextWriterStartElement(xmlWriter, history.second);
          writeArray(xmlWriter, history.first,
            [](xmlTextWriterPtr xmlWriter, const Stats::Inventory<>& el) {
              xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"amount",
                "%d", el.amount);
              xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"capacity",
                "%d", el.capacity);
            }
          );
        xmlTextWriterEndElement(xmlWriter);
      }
    xmlTextWriterEndElement(xmlWriter);
  xmlTextWriterEndElement(xmlWriter);
}

static void loadGlobals(xmlpp::TextReader& xmlReader, World& world,
  unsigned int ldsv_version
) {
  if(ldsv_version <= 2130) {
    loadGlobals_v2130(xmlReader, world, ldsv_version);
    return;
  }

  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "globals");
  [[used_in_assert]] int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    const std::string xml_tag = xmlReader.get_name();
    if(xml_tag == "given_scene")                      world.given_scene = xmlReader.read_inner_xml();
    else if(xml_tag == "total_time")                  world.total_time = std::stoi(xmlReader.read_inner_xml());
    else if(xml_tag == "people_pool")                 world.people_pool = std::stoi(xmlReader.read_inner_xml());
    else if(xml_tag == "total_money")                 world.stats.total_money = world.total_money = std::stoi(xmlReader.read_inner_xml());
    else if(xml_tag == "tech_level")                  world.stats.tech_level = world.tech_level = std::stoi(xmlReader.read_inner_xml());

    else if(xml_tag == "rockets_launched")            world.rockets_launched = std::stoi(xmlReader.read_inner_xml());
    else if(xml_tag == "rockets_launched_success")    world.rockets_launched_success = std::stoi(xmlReader.read_inner_xml());
    else if(xml_tag == "coal_survey_done")            world.coal_survey_done = std::stoi(xmlReader.read_inner_xml());
    else if(xml_tag == "game_end")                    world.gameEnd = std::stoi(xmlReader.read_inner_xml());

    else if(xml_tag == "money_rates") {
      if(!xmlReader.is_empty_element() && xmlReader.read())
      while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
        if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
          xmlReader.next();
          continue;
        }
        const std::string xml_tag = xmlReader.get_name();
        if(xml_tag == "income_tax_rate")          world.money_rates.income_tax = std::stoi(xmlReader.read_inner_xml());
        else if(xml_tag == "coal_tax_rate")       world.money_rates.coal_tax = std::stoi(xmlReader.read_inner_xml());
        else if(xml_tag == "dole_rate")           world.money_rates.dole = std::stoi(xmlReader.read_inner_xml());
        else if(xml_tag == "transport_cost_rate") world.money_rates.transport_cost = std::stoi(xmlReader.read_inner_xml());
        else if(xml_tag == "goods_tax_rate")      world.money_rates.goods_tax = std::stoi(xmlReader.read_inner_xml());
        else if(xml_tag == "export_tax_rate")     world.money_rates.export_tax = std::stoi(xmlReader.read_inner_xml());
        else if(xml_tag == "import_cost_rate")    world.money_rates.import_cost = std::stoi(xmlReader.read_inner_xml());
        else
          unexpectedXmlElement(xmlReader);
        xmlReader.next();
      }
    }

    else if(xml_tag == "taxable") {
      if(!xmlReader.is_empty_element() && xmlReader.read())
      while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
        if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
          xmlReader.next();
          continue;
        }
        const std::string xml_tag = xmlReader.get_name();
        if(xml_tag == "labor")         world.taxable.labor = std::stoi(xmlReader.read_inner_xml());
        else if(xml_tag == "coal")     world.taxable.coal = std::stoi(xmlReader.read_inner_xml());
        else if(xml_tag == "goods")    world.taxable.goods = std::stoi(xmlReader.read_inner_xml());
        else if(xml_tag == "trade_ex") world.taxable.trade_ex = std::stoi(xmlReader.read_inner_xml());
        else
          unexpectedXmlElement(xmlReader);
        xmlReader.next();
      }
    }

    else if(xml_tag == "tradeRules") {
      if(!xmlReader.is_empty_element() && xmlReader.read())
      while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
        if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
          xmlReader.next();
          continue;
        }
        std::smatch match;
        Commodity stuff;
        std::string xml_tag = xmlReader.get_name();
        if(std::regex_match(xml_tag, match,
            std::regex("(import|export)_(\\w+)_enable"))
          && (stuff = commodityFromStandardName(match[2].str().c_str())) !=
            STUFF_COUNT
        ) {
          CommodityRule& rule = world.tradeRule[stuff];
          bool& ixenable = match[1] == "import" ? rule.take : rule.give;
          ixenable = std::stoi(xmlReader.read_inner_xml());
        }
        else
          unexpectedXmlElement(xmlReader);
        xmlReader.next();
      }
    }

    else if(xml_tag == "stats") {
      if(!xmlReader.is_empty_element() && xmlReader.read())
      while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
        if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
          xmlReader.next();
          continue;
        }
        const std::string xml_tag = xmlReader.get_name();
        if(xml_tag == "highest_tech_level")   world.stats.highest_tech_level = std::stoi(xmlReader.read_inner_xml());
        else if(xml_tag == "total_pollution") world.stats.total_pollution = std::stoi(xmlReader.read_inner_xml());

        else if(xml_tag == "population") {
          if(!xmlReader.is_empty_element() && xmlReader.read())
          while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
            if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
              xmlReader.next();
              continue;
            }
            const std::string xml_tag = xmlReader.get_name();
            if(xml_tag == "population_lm")         world.stats.population.population_m.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "population_mtd")   world.stats.population.population_m.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "housed_lm")        world.stats.population.housed_m.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "housed_mtd")       world.stats.population.housed_m.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "starving_lm")      world.stats.population.starving_m.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "starving_mtd")     world.stats.population.starving_m.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "unemployed_lm")    world.stats.population.unemployed_m.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "unemployed_mtd")   world.stats.population.unemployed_m.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "housing_lm")       world.stats.population.housing_m.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "housing_mtd")      world.stats.population.housing_m.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "births_lm")        world.stats.population.births_m.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "births_mtd")       world.stats.population.births_m.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "deaths_lm")        world.stats.population.deaths_m.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "deaths_mtd")       world.stats.population.deaths_m.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "unnat_deaths_lm")  world.stats.population.unnat_deaths_m.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "unnat_deaths_mtd") world.stats.population.unnat_deaths_m.acc = std::stoi(xmlReader.read_inner_xml());

            else if(xml_tag == "evacuated_total")  world.stats.population.evacuated_t = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "births_total")     world.stats.population.births_t = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "pollution_deaths_total") world.stats.population.pollution_deaths_t = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "starve_deaths_total") world.stats.population.starve_deaths_t = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "unemployed_days_total") world.stats.population.unemployed_days_t = std::stoi(xmlReader.read_inner_xml());

            else if(xml_tag == "unemployed_history") world.stats.population.unemployed_history = std::stof(xmlReader.read_inner_xml());
            else if(xml_tag == "pollution_deaths_history") world.stats.population.pollution_deaths_history = std::stof(xmlReader.read_inner_xml());
            else if(xml_tag == "starve_deaths_history") world.stats.population.starve_deaths_history = std::stof(xmlReader.read_inner_xml());

            else if(xml_tag == "max_pop_ever")     world.stats.population.max_pop_ever = std::stoi(xmlReader.read_inner_xml());

            else
              unexpectedXmlElement(xmlReader);
            xmlReader.next();
          }
        }

        else if(xml_tag == "income") {
          if(!xmlReader.is_empty_element() && xmlReader.read())
          while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
            if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
              xmlReader.next();
              continue;
            }
            const std::string xml_tag = xmlReader.get_name();
            if(xml_tag == "income_tax_ly")       world.stats.income.income_tax.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "income_tax_ytd") world.stats.income.income_tax.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "coal_tax_ly")    world.stats.income.coal_tax.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "coal_tax_ytd")   world.stats.income.coal_tax.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "goods_tax_ly")   world.stats.income.goods_tax.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "goods_tax_ytd")  world.stats.income.goods_tax.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "export_tax_ly")  world.stats.income.export_tax.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "export_tax_ytd") world.stats.income.export_tax.acc = std::stoi(xmlReader.read_inner_xml());
            else
              unexpectedXmlElement(xmlReader);
            xmlReader.next();
          }
        }

        else if(xml_tag == "expenses") {
          if(!xmlReader.is_empty_element() && xmlReader.read())
          while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
            if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
              xmlReader.next();
              continue;
            }
            const std::string xml_tag = xmlReader.get_name();
            if(xml_tag == "construction_ly")       world.stats.expenses.construction.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "construction_ytd") world.stats.expenses.construction.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "coalSurvey_ly")    world.stats.expenses.coalSurvey.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "coalSurvey_ytd")   world.stats.expenses.coalSurvey.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "import_ly")        world.stats.expenses.import.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "import_ytd")       world.stats.expenses.import.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "unemployment_ly")  world.stats.expenses.unemployment.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "unemployment_ytd") world.stats.expenses.unemployment.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "transport_ly")     world.stats.expenses.transport.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "transport_ytd")    world.stats.expenses.transport.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "windmill_ly")      world.stats.expenses.windmill.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "windmill_ytd")     world.stats.expenses.windmill.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "university_ly")    world.stats.expenses.university.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "university_ytd")   world.stats.expenses.university.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "recycle_ly")       world.stats.expenses.recycle.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "recycle_ytd")      world.stats.expenses.recycle.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "deaths_ly")        world.stats.expenses.deaths.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "deaths_ytd")       world.stats.expenses.deaths.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "health_ly")        world.stats.expenses.health.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "health_ytd")       world.stats.expenses.health.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "rockets_ly")       world.stats.expenses.rockets.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "rockets_ytd")      world.stats.expenses.rockets.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "school_ly")        world.stats.expenses.school.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "school_ytd")       world.stats.expenses.school.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "firestation_ly")   world.stats.expenses.firestation.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "firestation_ytd")  world.stats.expenses.firestation.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "cricket_ly")       world.stats.expenses.cricket.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "cricket_ytd")      world.stats.expenses.cricket.acc = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "interest_ly")      world.stats.expenses.interest.stat = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "interest_ytd")     world.stats.expenses.interest.acc = std::stoi(xmlReader.read_inner_xml());
            else
              unexpectedXmlElement(xmlReader);
            xmlReader.next();
          }
        }

        else if(xml_tag == "sustainability") {
          if(!xmlReader.is_empty_element() && xmlReader.read())
          while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
            if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
              xmlReader.next();
              continue;
            }
            const std::string xml_tag = xmlReader.get_name();
            if(xml_tag == "mining_flag")           world.stats.sustainability.mining_flag = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "mining_years")     world.stats.sustainability.mining_years = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "trade_flag")       world.stats.sustainability.trade_flag = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "trade_years")      world.stats.sustainability.trade_years = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "old_money")        world.stats.sustainability.old_money = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "money_years")      world.stats.sustainability.money_years = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "old_population")   world.stats.sustainability.old_population = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "population_years") world.stats.sustainability.population_years = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "old_tech")         world.stats.sustainability.old_tech = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "tech_years")       world.stats.sustainability.tech_years = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "fire_years")       world.stats.sustainability.fire_years = std::stoi(xmlReader.read_inner_xml());
            else if(xml_tag == "sustainable")      world.stats.sustainability.sustainable = std::stoi(xmlReader.read_inner_xml());
            else
              unexpectedXmlElement(xmlReader);
            xmlReader.next();
          }
        }

        else if(xml_tag == "history") {
          if(!xmlReader.is_empty_element() && xmlReader.read())
          while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
            if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
              xmlReader.next();
              continue;
            }

            const std::unordered_map<std::string, std::deque<int>&> histories({
              {"pop", world.stats.history.pop},
              {"starve", world.stats.history.starve},
              {"nojobs", world.stats.history.nojobs},
              {"ppool", world.stats.history.ppool},
              {"tech", world.stats.history.tech},
              {"money", world.stats.history.money},
              {"pollution", world.stats.history.pollution},
              {"births", world.stats.history.births},
              {"deaths", world.stats.history.deaths},
              {"unnat_deaths", world.stats.history.unnat_deaths}
            });
            if(auto historyIt = histories.find(xmlReader.get_name());
              historyIt != histories.end()
            ) {
              std::deque<int>& history = historyIt->second;
              readArray(xmlReader, history,
                [](xmlpp::TextReader& xmlReader, int& el) {
                  el = std::stoi(xmlReader.read_inner_xml());
                }
              );
            }
            else if(Commodity stuff =
                commodityFromStandardName(xmlReader.get_name().c_str());
              stuff != STUFF_COUNT || xmlReader.get_name() == "tenants"
            ) {
              std::deque<Stats::Inventory<>>& history = stuff != STUFF_COUNT
                ? world.stats.history.inventory[stuff]
                : world.stats.history.tenants;
              readArray(xmlReader, history,
                [](xmlpp::TextReader& xmlReader, Stats::Inventory<>& el) {
                  if(!xmlReader.is_empty_element() && xmlReader.read())
                  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
                    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
                      xmlReader.next();
                      continue;
                    }

                    if(xmlReader.get_name() == "amount")
                      el.amount = std::stoi(xmlReader.read_inner_xml());
                    else if(xmlReader.get_name() == "capacity")
                      el.capacity = std::stoi(xmlReader.read_inner_xml());
                    else
                      unexpectedXmlElement(xmlReader);
                    xmlReader.next();
                  }
                }
              );
            }
            else {
              unexpectedXmlElement(xmlReader);
            }
            xmlReader.next();
          }
        }

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
  assert(xmlReader.get_name() == "globals");
  assert(xmlReader.get_depth() == depth);
}

static void loadGlobals_v2130(xmlpp::TextReader& xmlReader, World& world,
  unsigned int ldsv_version
) {
  int uncounted_import = 0;
  int uncounted_unemployment = 0;
  int uncounted_transport = 0;
  int uncounted_windmill = 0;
  int uncounted_university = 0;
  int uncounted_recycle = 0;
  int uncounted_deaths = 0;
  int uncounted_health = 0;
  int uncounted_rockets = 0;
  int uncounted_school = 0;
  int uncounted_firestation = 0;
  int uncounted_cricket = 0;

  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "globals");
  [[used_in_assert]] int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    const std::string xml_tag = xmlReader.get_name();
    const std::string xml_val = xmlReader.read_inner_xml();
    if(xml_tag == "given_scene")                      world.given_scene = xml_val;
    else if(xml_tag == "global_aridity")              ; // removed
    else if(xml_tag == "global_mountainity")          ; // removed
    else if(xml_tag == "world_side_len")              world.map = Map(std::stoi(xml_val));
    else if(xml_tag == "world_id")                    ; // removed
    else if(xml_tag == "old_setup_ground")            ; // removed
    else if(xml_tag == "climate")                     ; // removed

    else if(xml_tag == "main_screen_originx")         world.map.recentPoint.x = std::stoi(xml_val);
    else if(xml_tag == "main_screen_originy")         world.map.recentPoint.y = std::stoi(xml_val);
    else if(xml_tag == "total_time")                  world.total_time = std::stoi(xml_val);

    else if(xml_tag == "people_pool")                 world.people_pool = std::stoi(xml_val);
    else if(xml_tag == "total_money")                 world.stats.total_money = world.total_money = std::stoi(xml_val);
    else if(xml_tag == "ly_income_tax")               world.stats.income.income_tax.stat = std::stoi(xml_val);
    else if(xml_tag == "income_tax")                  world.taxable.labor = std::stoi(xml_val);
    else if(xml_tag == "income_tax_rate")             world.money_rates.income_tax = std::stoi(xml_val);

    else if(xml_tag == "ly_interest")                 world.stats.expenses.interest.stat = std::stoi(xml_val);
    else if(xml_tag == "ly_coal_tax")                 world.stats.income.coal_tax.stat = std::stoi(xml_val);
    else if(xml_tag == "coal_tax")                    world.taxable.coal = std::stoi(xml_val);
    else if(xml_tag == "coal_tax_rate")               world.money_rates.coal_tax = std::stoi(xml_val);

    else if(xml_tag == "ly_unemployment_cost")        world.stats.expenses.unemployment.stat = std::stoi(xml_val);
    else if(xml_tag == "unemployment_cost")           uncounted_unemployment = std::stoi(xml_val);
    else if(xml_tag == "dole_rate")                   world.money_rates.dole = std::stoi(xml_val);

    else if(xml_tag == "ly_transport_cost")           world.stats.expenses.transport.stat = std::stoi(xml_val);
    else if(xml_tag == "transport_cost")              uncounted_transport = std::stoi(xml_val);
    else if(xml_tag == "transport_cost_rate")         world.money_rates.transport_cost = std::stoi(xml_val);
    else if(xml_tag == "ly_goods_tax")                world.stats.income.goods_tax.stat = std::stoi(xml_val);
    else if(xml_tag == "goods_tax")                   world.taxable.goods = std::stoi(xml_val);
    else if(xml_tag == "goods_tax_rate")              world.money_rates.goods_tax = std::stoi(xml_val);

    else if(xml_tag == "ly_export_tax")               world.stats.income.export_tax.stat = std::stoi(xml_val);
    else if(xml_tag == "export_tax")                  world.taxable.trade_ex = std::stoi(xml_val);
    else if(xml_tag == "export_tax_rate")             world.money_rates.export_tax = std::stoi(xml_val);
    else if(xml_tag == "ly_import_cost")              world.stats.expenses.import.stat = std::stoi(xml_val);
    else if(xml_tag == "import_cost")                 uncounted_import = std::stoi(xml_val);
    else if(xml_tag == "import_cost_rate")            world.money_rates.import_cost = std::stoi(xml_val);

    else if(xml_tag == "ly_university_cost")          world.stats.expenses.university.stat = std::stoi(xml_val);
    else if(xml_tag == "university_cost")             uncounted_university = std::stoi(xml_val);
    else if(xml_tag == "ly_recycle_cost")             world.stats.expenses.recycle.stat = std::stoi(xml_val);
    else if(xml_tag == "recycle_cost")                uncounted_recycle = std::stoi(xml_val);
    else if(xml_tag == "ly_school_cost")              world.stats.expenses.school.stat = std::stoi(xml_val);
    else if(xml_tag == "school_cost")                 uncounted_school = std::stoi(xml_val);

    else if(xml_tag == "ly_health_cost")              world.stats.expenses.health.stat = std::stoi(xml_val);
    else if(xml_tag == "health_cost")                 uncounted_health = std::stoi(xml_val);
    else if(xml_tag == "ly_deaths_cost")              world.stats.expenses.deaths.stat = std::stoi(xml_val);
    else if(xml_tag == "deaths_cost")                 uncounted_deaths = std::stoi(xml_val);
    else if(xml_tag == "ly_rocket_pad_cost")          world.stats.expenses.rockets.stat = std::stoi(xml_val);
    else if(xml_tag == "rocket_pad_cost")             uncounted_rockets = std::stoi(xml_val);

    else if(xml_tag == "ly_windmill_cost")            world.stats.expenses.windmill.stat = std::stoi(xml_val);
    else if(xml_tag == "windmill_cost")               uncounted_windmill = std::stoi(xml_val);
    else if(xml_tag == "ly_fire_cost")                world.stats.expenses.firestation.stat = std::stoi(xml_val);
    else if(xml_tag == "fire_cost")                   uncounted_firestation = std::stoi(xml_val);
    else if(xml_tag == "ly_cricket_cost")             world.stats.expenses.cricket.stat = std::stoi(xml_val);
    else if(xml_tag == "cricket_cost")                uncounted_cricket = std::stoi(xml_val);

    else if(xml_tag == "tech_level")                  world.stats.tech_level = world.tech_level = std::stoi(xml_val);
    else if(xml_tag == "highest_tech_level")          world.stats.highest_tech_level = std::stoi(xml_val);
    else if(xml_tag == "tpopulation")                 world.stats.population.housed_m.acc = std::stoi(xml_val);
    else if(xml_tag == "thousing")                    world.stats.population.housing_m.acc = std::stoi(xml_val);
    else if(xml_tag == "tstarving_population")        world.stats.population.starving_m.acc = std::stoi(xml_val);
    else if(xml_tag == "tunemployed_population")      world.stats.population.unemployed_m.acc = std::stoi(xml_val);

    else if(xml_tag == "total_pollution")             world.stats.total_pollution = std::stoi(xml_val);
    else if(xml_tag == "rockets_launched")            world.rockets_launched = std::stoi(xml_val);
    else if(xml_tag == "rockets_launched_success")    world.rockets_launched_success = std::stoi(xml_val);

    else if(xml_tag == "coal_survey_done")            world.coal_survey_done = std::stoi(xml_val);
    else if(xml_tag == "cheat_flag")                  ;
    else if(xml_tag == "total_pollution_deaths")      world.stats.population.pollution_deaths_t = std::stoi(xml_val);
    else if(xml_tag == "pollution_deaths_history")    world.stats.population.pollution_deaths_history = std::stof(xml_val);
    else if(xml_tag == "total_starve_deaths")         world.stats.population.starve_deaths_t = std::stoi(xml_val);
    else if(xml_tag == "starve_deaths_history")       world.stats.population.starve_deaths_history = std::stof(xml_val);

    else if(xml_tag == "total_unemployed_days")       world.stats.population.unemployed_days_t += std::stoi(xml_val);
    else if(xml_tag == "total_unemployed_years")      world.stats.population.unemployed_days_t += std::stoi(xml_val) * NUMOF_DAYS_IN_YEAR; // deprecated
    else if(xml_tag == "unemployed_history")          world.stats.population.unemployed_history = std::stof(xml_val);
    else if(xml_tag == "max_pop_ever")                world.stats.population.max_pop_ever = std::stoi(xml_val);
    else if(xml_tag == "total_evacuated")             world.stats.population.evacuated_t = std::stoi(xml_val);
    else if(xml_tag == "total_births")                world.stats.population.births_t = std::stoi(xml_val);

    else if(xml_tag == "sust_dig_ore_coal_tip_flag")  world.stats.sustainability.mining_flag = std::stoi(xml_val);
    else if(xml_tag == "sust_dig_ore_coal_count")     world.stats.sustainability.mining_years = std::stoi(xml_val);
    else if(xml_tag == "sust_port_count")             world.stats.sustainability.trade_years = std::stoi(xml_val);
    else if(xml_tag == "sust_old_money_count")        world.stats.sustainability.money_years = std::stoi(xml_val);
    else if(xml_tag == "sust_old_population_count")   world.stats.sustainability.population_years = std::stoi(xml_val);
    else if(xml_tag == "sust_old_tech_count")         world.stats.sustainability.tech_years = std::stoi(xml_val);
    else if(xml_tag == "sust_fire_count")             world.stats.sustainability.fire_years = std::stoi(xml_val);
    else if(xml_tag == "sust_old_money")              world.stats.sustainability.old_money = std::stoi(xml_val);
    else if(xml_tag == "sust_port_flag")              world.stats.sustainability.trade_flag = std::stoi(xml_val);
    else if(xml_tag == "sust_old_population")         world.stats.sustainability.old_population = std::stoi(xml_val);
    else if(xml_tag == "sust_old_tech")               world.stats.sustainability.old_tech = std::stoi(xml_val);
    else if(xml_tag == "sustain_flag")                world.stats.sustainability.sustainable = std::stoi(xml_val);

    else if(xml_tag == "monthgraph_pop")              readArray_old(xmlReader, world.stats.history.pop);
    else if(xml_tag == "monthgraph_starve")           readArray_old(xmlReader, world.stats.history.starve);
    else if(xml_tag == "monthgraph_nojobs")           readArray_old(xmlReader, world.stats.history.nojobs);
    else if(xml_tag == "monthgraph_ppool")            readArray_old(xmlReader, world.stats.history.ppool);
    else if(xml_tag == "pbar") {
      int id = std::stoi(xmlReader.get_attribute("id"));
      switch(id) {
      case 0: break; // pop
      case 1: readPbar_old(xmlReader, world.stats.history.tech); break;
      case 2: readPbar_old(xmlReader, world.stats.history.money); break;
      case 3: readPbar_old(xmlReader, world.stats.history.inventory[STUFF_FOOD]); break;
      case 4: readPbar_old(xmlReader, world.stats.history.inventory[STUFF_LABOR]); break;
      case 5: readPbar_old(xmlReader, world.stats.history.inventory[STUFF_GOODS]); break;
      case 6: readPbar_old(xmlReader, world.stats.history.inventory[STUFF_COAL]); break;
      case 7: readPbar_old(xmlReader, world.stats.history.inventory[STUFF_ORE]); break;
      case 8: readPbar_old(xmlReader, world.stats.history.inventory[STUFF_STEEL]); break;
      case 9: readPbar_old(xmlReader, world.stats.history.pollution); break;
      case 10: readPbar_old(xmlReader, world.stats.history.inventory[STUFF_LOVOLT]); break;
      case 11: readPbar_old(xmlReader, world.stats.history.inventory[STUFF_HIVOLT]); break;
      case 12: readPbar_old(xmlReader, world.stats.history.inventory[STUFF_WATER]); break;
      case 13: readPbar_old(xmlReader, world.stats.history.inventory[STUFF_WASTE]); break;
      case 14: readPbar_old(xmlReader, world.stats.history.tenants); break;
      default:
        std::cerr << "warning: skipping invalid pbar id: " << id << std::endl
          << "  Someone may be trying something nasty."
          << " See https://github.com/lincity-ng/lincity-ng/issues/205"
          << " for more information." << std::endl;
        break;
      }
    }
    else goto more_globals; goto found_global; more_globals:

    for(Commodity c = STUFF_INIT; c < STUFF_COUNT; c++) {
      bool *ixenable = NULL;
      const char * const &cname = commodityNames[c];
      if(xml_tag == std::string("import_") + cname + "_enable")
        ixenable = &world.tradeRule[c].take;
      else if(xml_tag == std::string("export_") + cname + "_enable")
        ixenable = &world.tradeRule[c].give;
      else continue;

      *ixenable = std::stoi(xml_val);
      goto found_global;
    }

    unexpectedXmlElement(xmlReader);

    found_global: ;
    xmlReader.next();
  }
  assert(xmlReader.get_name() == "globals");
  assert(xmlReader.get_depth() == depth);

  world.gameEnd = false;

  world.expense(uncounted_import * world.money_rates.import_cost / 100,
    world.stats.expenses.import);
  world.expense(uncounted_unemployment * world.money_rates.dole / 100,
    world.stats.expenses.unemployment);
  world.expense(uncounted_transport * world.money_rates.transport_cost / 100,
    world.stats.expenses.transport);
  world.expense(uncounted_windmill, world.stats.expenses.windmill);
  world.expense(uncounted_university, world.stats.expenses.university);
  world.expense(uncounted_recycle, world.stats.expenses.recycle);
  world.expense(uncounted_deaths, world.stats.expenses.deaths);
  world.expense(uncounted_health * world.tech_level / 10000,
    world.stats.expenses.health);
  world.expense(uncounted_rockets, world.stats.expenses.rockets);
  world.expense(uncounted_school, world.stats.expenses.school);
  world.expense(uncounted_firestation * world.tech_level / 10000,
    world.stats.expenses.firestation);
  world.expense(uncounted_cricket, world.stats.expenses.cricket);

  world.stats.population.population_m.acc++; // don't lose the game immediately
}

static void saveMap(xmlTextWriterPtr xmlWriter, const Map& map) {
  xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"size", "%d", map.len());

  for(const MapTile& tile : map) {
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"MapTile");
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"group", "%d", tile.group);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-x", "%d", tile.point.x);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-y", "%d", tile.point.y);
      saveMapTile(xmlWriter, tile);
    xmlTextWriterEndElement(xmlWriter);
  }

  for(Construction *cst : map.constructions) {
    if(cst->isDead()) continue;
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"Construction");
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"group", "%d", cst->constructionGroup->group);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-x", "%d", cst->point.x);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-y", "%d", cst->point.y);
      cst->save(xmlWriter);
    xmlTextWriterEndElement(xmlWriter);
  }

  xmlTextWriterStartElement(xmlWriter, (xmlStr)"recentPoint");
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"x", "%d", map.recentPoint.x);
    xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"y", "%d", map.recentPoint.y);
  xmlTextWriterEndElement(xmlWriter);
}

static void loadMap(xmlpp::TextReader& xmlReader, World& world,
  unsigned int ldsv_version
) {
  Map& map = world.map;
  std::list<std::pair<Construction *, MapPoint>> constructions;

  if(ldsv_version > 2130) {
    int mapSize = std::stoi(xmlReader.get_attribute("size"));
    map = Map(mapSize);
  }

  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "map");
  [[used_in_assert]] int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    if(xmlReader.get_name() == "MapTile") {
      unsigned short group = std::stoi(xmlReader.get_attribute("group"));
      MapPoint point(
        std::stoi(xmlReader.get_attribute("map-x")),
        std::stoi(xmlReader.get_attribute("map-y"))
      );
      if(!map.is_inside(point))
        throw std::runtime_error("a MapTile seems to be outside the map");
      MapTile& tile = *map(point);
      tile.group = group;
      loadMapTile(xmlReader, tile, ldsv_version);
    }
    else if(xmlReader.get_name() == "Construction") {
      int group = std::stoi(xmlReader.get_attribute("group"));
      MapPoint point(
        std::stoi(xmlReader.get_attribute("map-x")),
        std::stoi(xmlReader.get_attribute("map-y"))
      );
      ConstructionGroup *cstgrp =
        ConstructionGroup::getConstructionGroup(group);
      if(!cstgrp)
        throw std::runtime_error("invalid group");
      Construction *cst = cstgrp->createConstruction(world);
      cst->load(xmlReader, ldsv_version);
      constructions.emplace_back(cst, point);
    }
    else if(xmlReader.get_name() == "recentPoint" && ldsv_version > 2130) {
      bool found_x = false, found_y = false;
      if(!xmlReader.is_empty_element() && xmlReader.read())
      while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
        if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
          xmlReader.next();
          continue;
        }
        if(xmlReader.get_name() == "x") {
          map.recentPoint.x = std::stoi(xmlReader.read_inner_xml());
          found_x = true;
        }
        else if(xmlReader.get_name() == "y") {
          map.recentPoint.y = std::stoi(xmlReader.read_inner_xml());
          found_y = true;
        }
        else
          unexpectedXmlElement(xmlReader);
        xmlReader.next();
      }
      if(!found_x) missingXmlElement(xmlReader, "x");
      if(!found_y) missingXmlElement(xmlReader, "y");
    }
    else
      unexpectedXmlElement(xmlReader);

    xmlReader.next();
  }
  assert(xmlReader.get_name() == "map");
  assert(xmlReader.get_depth() == depth);

  for(auto& cst : constructions) {
    cst.first->place(cst.second);
  }

  map.alt_min = map.alt_max = map(MapPoint(0,0))->ground.altitude;
  for(const MapTile& tile : map) {
    map.alt_min = std::min(map.alt_min, tile.ground.altitude);
    map.alt_max = std::max(map.alt_max, tile.ground.altitude);
  }
  map.alt_step = (map.alt_max - map.alt_min) / 10;

  map.connect_transport(1, 1, map.len() - 2, map.len() - 2);
  map.desert_water_frontiers(0, 0, map.len(), map.len());
}

static void saveMapTile(xmlTextWriterPtr xmlWriter, const MapTile& tile) {
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"flags",    "0x%x", tile.flags & ~VOLATILE_FLAGS);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"type",       "%d", tile.type);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"air_pol",    "%d", tile.pollution);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ore",        "%d", tile.ore_reserve);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal",       "%d", tile.coal_reserve);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"altitude",   "%d", tile.ground.altitude);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ecotable",   "%d", tile.ground.ecotable);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"wastes",     "%d", tile.ground.wastes);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"grd_pol",    "%d", tile.ground.pollution);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"water_alt",  "%d", tile.ground.water_alt);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"water_pol",  "%d", tile.ground.water_pol);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"water_wast", "%d", tile.ground.water_wast);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"water_next", "%d", tile.ground.water_next);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"int1",       "%d", tile.ground.int1);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"int2",       "%d", tile.ground.int2);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"int3",       "%d", tile.ground.int3);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"int4",       "%d", tile.ground.int4);
}

static void loadMapTile(xmlpp::TextReader& xmlReader, MapTile& tile,
  unsigned int ldsv_version
) {
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "MapTile");
  [[used_in_assert]] int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    std::string xml_tag = xmlReader.get_name();
    std::string xml_val = xmlReader.read_inner_xml();
    if     (xml_tag == "flags") {    tile.flags             = std::stoul(xml_val, NULL, 0) & ~VOLATILE_FLAGS;
      if(ldsv_version <= 2130) {
        tile.flags = 0
          | (tile.flags & 0x00000100 ? FLAG_MARKET_COVER | FLAG_MARKET_COVER_CHECK : 0)
          | (tile.flags & 0x00100000 ? FLAG_FIRE_COVER | FLAG_FIRE_COVER_CHECK : 0)
          | (tile.flags & 0x00200000 ? FLAG_HEALTH_COVER | FLAG_HEALTH_COVER_CHECK : 0)
          | (tile.flags & 0x00400000 ? FLAG_CRICKET_COVER | FLAG_CRICKET_COVER_CHECK : 0)
          | (tile.flags & 0x00800000 ? FLAG_IS_RIVER : 0)
          | (tile.flags & 0x01000000 ? FLAG_IS_LAKE : 0)
          // | (tile.flags & 0x08000000 ? FLAG_INVISIBLE : 0)
          | (tile.flags & 0x20000000 ? FLAG_HAS_UNDERGROUND_WATER : 0);
      }
    }
    else if(xml_tag == "type")       tile.type              = std::stoi(xml_val);
    else if(xml_tag == "air_pol")    tile.pollution         = std::stoi(xml_val);
    else if(xml_tag == "ore")        tile.ore_reserve       = std::stoi(xml_val);
    else if(xml_tag == "coal")       tile.coal_reserve      = std::stoi(xml_val);

    else if(xml_tag == "altitude")   tile.ground.altitude   = std::stoi(xml_val);
    else if(xml_tag == "ecotable")   tile.ground.ecotable   = std::stoi(xml_val);
    else if(xml_tag == "wastes")     tile.ground.wastes     = std::stoi(xml_val);
    else if(xml_tag == "grd_pol")    tile.ground.pollution  = std::stoi(xml_val);
    else if(xml_tag == "water_alt")  tile.ground.water_alt  = std::stoi(xml_val);
    else if(xml_tag == "water_pol")  tile.ground.water_pol  = std::stoi(xml_val);
    else if(xml_tag == "water_wast") tile.ground.water_wast = std::stoi(xml_val);
    else if(xml_tag == "water_next") tile.ground.water_next = std::stoi(xml_val);
    else if(xml_tag == "int1")       tile.ground.int1       = std::stoi(xml_val);
    else if(xml_tag == "int2")       tile.ground.int2       = std::stoi(xml_val);
    else if(xml_tag == "int3")       tile.ground.int3       = std::stoi(xml_val);
    else if(xml_tag == "int4")       tile.ground.int4       = std::stoi(xml_val);
    else unexpectedXmlElement(xmlReader);

    xmlReader.next();
  }
  assert(xmlReader.get_name() == "MapTile");
  assert(xmlReader.get_depth() == depth);

  tile.flags &= ~VOLATILE_FLAGS;
}

template<typename A>
static void writeArray(xmlTextWriterPtr xmlWriter, const A& array,
  std::function<void(xmlTextWriterPtr, const typename A::value_type&)>
    writeElement
) {
  xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"size", "%zu",
    array.size());
  for(int i = 0; i < array.size(); i++) {
    xmlTextWriterStartElement(xmlWriter,
      (xmlStr)(std::ostringstream() << "index_" << i).str().c_str()
    );
      writeElement(xmlWriter, array.at(i));
    xmlTextWriterEndElement(xmlWriter);
  }
}

template<typename A>
static void readArray(xmlpp::TextReader& xmlReader, A& array,
  std::function<void(xmlpp::TextReader&, typename A::value_type&)> readElement
) {
  // array.resize(std::stoull(xmlReader.get_attribute("size")));
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  [[used_in_assert]] int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }
    if(xmlReader.get_name().substr(0, 6) != "index_") {
      unexpectedXmlElement(xmlReader);
      xmlReader.next();
      continue;
    }
    typename A::value_type *element = nullptr;
    try {
      element = &array.at(std::stoull(xmlReader.get_name().substr(6)));
    } catch(std::invalid_argument ex) {
      unexpectedXmlElement(xmlReader);
      xmlReader.next();
      continue;
    } catch(std::out_of_range ex) {
      unexpectedXmlElement(xmlReader);
      xmlReader.next();
      continue;
    }
    assert(element);
    readElement(xmlReader, *element);
    xmlReader.next();
  }
  assert(xmlReader.get_depth() == depth);
}

static void readArray_old(xmlpp::TextReader& xmlReader, std::deque<int>& array) {
  std::string str = xmlReader.read_inner_xml();
  while(!str.empty() && std::isspace(static_cast<unsigned char>(str.back())))
    str.erase(str.size() - 1);
  array.clear();
  while(!str.empty()) {
    std::size_t charcount;
    array.push_back(std::stoi(str, &charcount));
    str.erase(0, charcount);
  }
}

static void readPbar_old(xmlpp::TextReader& xmlReader, std::deque<int>& array) {
  size_t arraySize = array.size();
  int diff = 0;
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "pbar");
  [[used_in_assert]] int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    if(xmlReader.get_name() == "diff")
      diff = std::stoi(xmlReader.read_inner_xml());
    else if(xmlReader.get_name() == "data") {
      std::string str = xmlReader.read_inner_xml();
      while(!str.empty() && std::isspace(static_cast<unsigned char>(str.back())))
        str.erase(str.size() - 1);
      array.clear();
      while(!str.empty()) {
        std::size_t charcount;
        array.push_front(std::stoi(str, &charcount));
        str.erase(0, charcount);
      }
    }
    else
      unexpectedXmlElement(xmlReader);

    xmlReader.next();
  }
  assert(xmlReader.get_name() == "pbar");
  assert(xmlReader.get_depth() == depth);

  assert(!array.empty());
  array.push_back(array.front() - diff);
  array.resize(arraySize);
}

static void readPbar_old(xmlpp::TextReader& xmlReader,
  std::deque<Stats::Inventory<>>& array
) {
  size_t arraySize = array.size();
  int diff = 0;
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "pbar");
  [[used_in_assert]] int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    if(xmlReader.get_name() == "diff")
      diff = std::stoi(xmlReader.read_inner_xml());
    else if(xmlReader.get_name() == "data") {
      std::string str = xmlReader.read_inner_xml();
      while(!str.empty() && std::isspace(static_cast<unsigned char>(str.back())))
        str.erase(str.size() - 1);
      array.clear();
      while(!str.empty()) {
        std::size_t charcount;
        array.push_front((Stats::Inventory<>){
          .amount = std::stoi(str, &charcount),
          .capacity = 1000
        });
        str.erase(0, charcount);
      }
    }
    else
      unexpectedXmlElement(xmlReader);

    xmlReader.next();
  }
  assert(xmlReader.get_name() == "pbar");
  assert(xmlReader.get_depth() == depth);

  assert(!array.empty());
  array.push_back((Stats::Inventory<>){
    .amount = array.front().amount - diff,
    .capacity = 1000
  });
  array.resize(arraySize);
}
