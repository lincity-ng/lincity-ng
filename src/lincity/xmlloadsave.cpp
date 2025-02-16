/* ---------------------------------------------------------------------- *
 * src/lincity/xmlloadsave.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2024 David Bears <dbear4q@gmail.com>
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

#include "xmlloadsave.h"

#include <libxml++/parsers/textreader.h>   // for TextReader
#include <libxml/parser.h>                 // for XML_PARSE_NONET, xmlOutput...
#include <libxml/xmlIO.h>                  // for xmlOutputBufferClose, xmlO...
#include <libxml/xmlerror.h>               // for XML_ERR_OK
#include <libxml/xmlreader.h>              // for xmlReaderForIO, xmlTextRea...
#include <libxml/xmlversion.h>             // for LIBXML_VERSION
#include <libxml/xmlwriter.h>              // for xmlTextWriterWriteFormatEl...
#include <zlib.h>                          // for gzclose, gzFile, gzopen
#include <algorithm>                       // for max, min
#include <array>                           // for array
#include <cassert>                         // for assert
#include <cstring>                         // for NULL, strncpy
#include <iostream>                        // for basic_ostream, operator<<
#include <list>                            // for list, _List_iterator
#include <memory>                          // for shared_ptr
#include <sstream>                         // for basic_ostringstream
#include <stdexcept>                       // for runtime_error
#include <string>                          // for basic_string, char_traits
#include <utility>                         // for pair
#include <unordered_map>
#include <functional>

#include "ConstructionCount.h"             // for ConstructionCount
#include "commodities.hpp"                 // for Commodity, CommodityRule
#include "engglobs.h"                      // for world, alt_min, alt_max
#include "engine.h"                        // for desert_water_frontiers
#include "gui_interface/pbar_interface.h"  // for pbar_st, NUM_PBARS, PBAR_D...
#include "gui_interface/shared_globals.h"  // for monthgraph_size, main_scre...
#include "lin-city.h"                      // for VOLATILE_FLAGS
#include "lintypes.h"                      // for xmlTextWriterPtr, Construc...
#include "modules/port.h"                  // for PortConstructionGroup, por...
#include "modules/windmill.h"              // for MODERN_WINDMILL_TECH
#include "stats.h"                         // for ly_cricket_cost, ly_deaths...
#include "transport.h"                     // for connect_transport
#include "world.h"                         // for MapTile, Ground, Map

static void saveGlobals(xmlTextWriterPtr xmlWriter, const World& world);
static void loadGlobals(xmlpp::TextReader& xmlReader, World& World);
static void saveMap(xmlTextWriterPtr xmlWriter, const Map& map);
static void loadMap(xmlpp::TextReader& xmlReader, World& world);
static void saveMapTile(xmlTextWriterPtr xmlWriter, const MapTile& tile);
static void loadMapTile(xmlpp::TextReader& xmlReader, MapTile& tile);
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
World::save(const std::filesystem::path& filename) {
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
  xmlReader.read();

  // find the SaveGame node
  while(true) {
    if(xmlReader.get_read_state() == xmlpp::TextReader::ReadState::EndOfFile)
      throw std::runtime_error("failed to find lc-game element");
    else if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element);
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
    xmlReader.next();
  }

  // parse load/save version
  std::string versionStr = xmlReader.get_attribute("ldsv-version");
  if(versionStr.empty())
    throw std::runtime_error("failed to parse load/save version");
  int ldsv_version = std::stoi(versionStr);
  if(ldsv_version > LOADSAVE_VERSION_CURRENT)
    throw std::runtime_error("load/save version too new");
  else if(ldsv_version < LOADSAVE_VERSION_COMPAT)
    throw std::runtime_error("load/save version too old");

  std::unique_ptr<World> world(new World());

  // parse sections
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "lc-game");
  int depth = xmlReader.get_depth();
  assert(depth == 0);
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    if(xmlReader.get_name() == "globals") {
      loadGlobals(xmlReader, *world);
    }
    else if(xmlReader.get_name() == "map") {
      loadMap(xmlReader, *world);
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
  std::cerr << "warning: skipping unexpected element '"
    << xmlReader.get_name() << "'" << std::endl;
}

static void saveGlobals(xmlTextWriterPtr xmlWriter, const World& world) {

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"given_scene",                 "%s", world.given_scene);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"world_side_len",              "%d", world.map.len());
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"main_screen_originx",         "%d", world.map.recentPoint.x);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"main_screen_originy",         "%d", world.map.recentPoint.y);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_time",                  "%d", world.total_time);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"people_pool",                 "%d", world.people_pool);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_money",                 "%d", world.total_money);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_income_tax",               "%d", world.stats.income.income_tax.stat); // TODO: save acc
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"income_tax",                  "%d", world.stats.taxable.labor);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"income_tax_rate",             "%d", world.money_rates.income_tax);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_interest",                 "%d", world.stats.expenses.interest);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_coal_tax",                 "%d", world.stats.income.coal_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal_tax",                    "%d", world.stats.taxable.coal); // TODO: save acc
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal_tax_rate",               "%d", world.money_rates.coal_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_unemployment_cost",        "%d", world.stats.expenses.unemployment);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unemployment_cost",           "%d", world.stats.population.unemployed_m.acc + world.stats.population.unemployed_d.acc);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"dole_rate",                   "%d", world.money_rates.dole);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_transport_cost",           "%d", world.stats.expenses.transport);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"transport_cost",              "%d", 0);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"transport_cost_rate",         "%d", world.money_rates.transport_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_goods_tax",                "%d", world.stats.income.goods_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"goods_tax",                   "%d", world.stats.taxable.goods);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"goods_tax_rate",              "%d", world.money_rates.goods_tax);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_export_tax",               "%d", world.stats.income.export_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"export_tax",                  "%d", world.stats.taxable.trade_ex);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"export_tax_rate",             "%d", world.money_rates.export_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_import_cost",              "%d", world.stats.expenses.import);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"import_cost",                 "%d", 0);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"import_cost_rate",            "%d", world.money_rates.import_cost);

  for(Commodity c = STUFF_INIT; c < STUFF_COUNT; c++) {
    const char * const &cname = commodityNames[c];
    xmlTextWriterWriteFormatElement(xmlWriter,
      (xmlStr)(std::string("import_")+cname+"_enable").c_str(), "%d",
      world.tradeRule[c].take);
    xmlTextWriterWriteFormatElement(xmlWriter,
      (xmlStr)(std::string("export_")+cname+"_enable").c_str(), "%d",
      world.tradeRule[c].give);
  }

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_university_cost",          "%d", world.stats.expenses.university);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"university_cost",             "%d", 0);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_recycle_cost",             "%d", world.stats.expenses.recycle);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"recycle_cost",                "%d", 0);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_school_cost",              "%d", world.stats.expenses.school);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"school_cost",                 "%d", 0);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_health_cost",              "%d", world.stats.expenses.health);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"health_cost",                 "%d", 0);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_deaths_cost",              "%d", world.stats.expenses.deaths);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"deaths_cost",                 "%d", 0);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_rocket_pad_cost",          "%d", world.stats.expenses.rockets);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"rocket_pad_cost",             "%d", 0);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_windmill_cost",            "%d", world.stats.expenses.windmill);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"windmill_cost",               "%d", 0);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_fire_cost",                "%d", world.stats.expenses.firestation);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"fire_cost",                   "%d", 0);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_cricket_cost",             "%d", world.stats.expenses.cricket);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"cricket_cost",                "%d", 0);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tech_level",                  "%d", world.tech_level);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"highest_tech_level",          "%d", world.stats.highest_tech_level);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tpopulation",                 "%d", world.stats.population.housed_m);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"thousing",                    "%d", world.stats.population.housing_m);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tstarving_population",        "%d", world.stats.population.starving_m);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tunemployed_population",      "%d", world.stats.population.unemployed_m);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_pollution",             "%d", world.stats.total_pollution);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"rockets_launched",            "%d", world.rockets_launched);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"rockets_launched_success",    "%d", world.rockets_launched_success);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal_survey_done",            "%d", world.coal_survey_done);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"cheat_flag",                  "%d", cheat_flag);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_pollution_deaths",      "%d", world.stats.population.pollution_deaths_t);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"pollution_deaths_history",    "%f", world.stats.population.pollution_deaths_history);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_starve_deaths",         "%d", world.stats.population.starve_deaths_t);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"starve_deaths_history",       "%f", world.stats.population.starve_deaths_history);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_unemployed_days",       "%d", world.stats.population.unemployed_days_t);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unemployed_history",          "%f", world.stats.population.unemployed_history);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"max_pop_ever",                "%d", world.stats.population.max_pop_ever);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_evacuated",             "%d", world.stats.population.evacuated_t);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_births",                "%d", world.stats.population.births_t);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_dig_ore_coal_tip_flag",  "%d", world.stats.sustainability.mining_flag);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_dig_ore_coal_count",     "%d", world.stats.sustainability.mining_years);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_port_count",             "%d", world.stats.sustainability.trade_years);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_money_count",        "%d", world.stats.sustainability.money_years);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_population_count",   "%d", world.stats.sustainability.population_years);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_tech_count",         "%d", world.stats.sustainability.tech_years);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_fire_count",             "%d", world.stats.sustainability.fire_years);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_money",              "%d", world.stats.sustainability.old_money);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_port_flag",              "%d", world.stats.sustainability.trade_flag);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_population",         "%d", world.stats.sustainability.old_population);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_tech",               "%d", world.stats.sustainability.old_tech);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sustain_flag",                "%d", world.stats.sustainability.sustainable);

  xmlTextWriterStartElement(xmlWriter, (xmlStr)"history");
    std::vector<std::pair<std::deque<int>, xmlStr>> histories({
      {world.stats.history.pop, (xmlStr)"pop"},
      {world.stats.history.starve, (xmlStr)"starve"},
      {world.stats.history.nojobs, (xmlStr)"nojobs"},
      {world.stats.history.ppool, (xmlStr)"ppool"},
      {world.stats.history.tech, (xmlStr)"tech"},
      {world.stats.history.money, (xmlStr)"money"},
      {world.stats.history.pollution, (xmlStr)"pollution"}
    });
    for(auto& history : histories) {
      xmlTextWriterStartElement(xmlWriter, history.second);
        writeArray(xmlWriter, history.first,
          [](xmlTextWriterPtr xmlWriter, const int& el) {
            xmlTextWriterWriteFormatString(xmlWriter, "%d", el);
          }
        );
      xmlTextWriterEndElement(xmlWriter);
    }
    for(Commodity s = STUFF_INIT; s < STUFF_COUNT; s++) {
      xmlTextWriterStartElement(xmlWriter, (xmlStr)commodityStandardName(s));
        writeArray(xmlWriter, world.stats.history.inventory[s],
          [](xmlTextWriterPtr xmlWriter, const Stats::Inventory<>& el) {
            xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"amount", "%d",
              el.amount);
            xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"capacity", "%d",
              el.capacity);
          }
        );
      xmlTextWriterEndElement(xmlWriter);
    }
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"tenants");
      writeArray(xmlWriter, world.stats.history.tenants,
        [](xmlTextWriterPtr xmlWriter, const Stats::Inventory<>& el) {
          xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"amount", "%d",
            el.amount);
          xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"capacity", "%d",
            el.capacity);
        }
      );
    xmlTextWriterEndElement(xmlWriter);
  xmlTextWriterEndElement(xmlWriter);
}

static void loadGlobals(xmlpp::TextReader& xmlReader, World& world) {
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "globals");
  int depth = xmlReader.get_depth();
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
    else if(xml_tag == "global_aridity")              ; // deprecated
    else if(xml_tag == "global_mountainity")          ; // deprecated
    else if(xml_tag == "world_side_len")              world.map = Map(std::stoi(xml_val)); // TODO: move to loadMap
    else if(xml_tag == "world_id")                    ; // deprecated
    else if(xml_tag == "old_setup_ground")            ; // deprecated
    else if(xml_tag == "climate")                     ; // deprecated

    else if(xml_tag == "main_screen_originx")         world.map.recentPoint.x = std::stoi(xml_val);
    else if(xml_tag == "main_screen_originy")         world.map.recentPoint.y = std::stoi(xml_val);
    else if(xml_tag == "total_time")                  world.total_time = std::stoi(xml_val);

    else if(xml_tag == "people_pool")                 world.people_pool = std::stoi(xml_val);
    else if(xml_tag == "total_money")                 world.stats.total_money = world.total_money = std::stoi(xml_val);
    else if(xml_tag == "ly_income_tax")               world.stats.income.income_tax.stat = std::stoi(xml_val);
    else if(xml_tag == "income_tax")                  world.stats.taxable.labor = std::stoi(xml_val);
    else if(xml_tag == "income_tax_rate")             world.money_rates.income_tax = std::stoi(xml_val);

    else if(xml_tag == "ly_interest")                 world.stats.expenses.interest.stat = std::stoi(xml_val);
    else if(xml_tag == "ly_coal_tax")                 world.stats.income.coal_tax.stat = std::stoi(xml_val);
    else if(xml_tag == "coal_tax")                    world.stats.taxable.coal = std::stoi(xml_val);
    else if(xml_tag == "coal_tax_rate")               world.money_rates.coal_tax = std::stoi(xml_val);

    else if(xml_tag == "ly_unemployment_cost")        world.stats.expenses.unemployment.stat = std::stoi(xml_val);
    else if(xml_tag == "unemployment_cost")           world.stats.expenses.unemployment.acc = std::stoi(xml_val);
    else if(xml_tag == "dole_rate")                   world.money_rates.dole = std::stoi(xml_val);

    else if(xml_tag == "ly_transport_cost")           world.stats.expenses.transport.stat = std::stoi(xml_val);
    else if(xml_tag == "transport_cost")              world.stats.expenses.transport.acc = std::stoi(xml_val);
    else if(xml_tag == "transport_cost_rate")         world.money_rates.transport_cost = std::stoi(xml_val);
    else if(xml_tag == "ly_goods_tax")                world.stats.income.goods_tax.stat = std::stoi(xml_val);
    else if(xml_tag == "goods_tax")                   world.stats.taxable.goods = std::stoi(xml_val);
    else if(xml_tag == "goods_tax_rate")              world.money_rates.goods_tax = std::stoi(xml_val);

    else if(xml_tag == "ly_export_tax")               world.stats.income.export_tax.stat = std::stoi(xml_val);
    else if(xml_tag == "export_tax")                  world.stats.taxable.trade_ex = std::stoi(xml_val);
    else if(xml_tag == "export_tax_rate")             world.money_rates.export_tax = std::stoi(xml_val);
    else if(xml_tag == "ly_import_cost")              world.stats.expenses.import.stat = std::stoi(xml_val);
    else if(xml_tag == "import_cost")                 world.stats.expenses.import.acc = std::stoi(xml_val);
    else if(xml_tag == "import_cost_rate")            world.money_rates.import_cost = std::stoi(xml_val);

    else if(xml_tag == "ly_university_cost")          world.stats.expenses.university.stat = std::stoi(xml_val);
    else if(xml_tag == "university_cost")             world.stats.expenses.university.acc = std::stoi(xml_val);
    else if(xml_tag == "ly_recycle_cost")             world.stats.expenses.recycle.stat = std::stoi(xml_val);
    else if(xml_tag == "recycle_cost")                world.stats.expenses.recycle.acc = std::stoi(xml_val);
    else if(xml_tag == "ly_school_cost")              world.stats.expenses.school.stat = std::stoi(xml_val);
    else if(xml_tag == "school_cost")                 world.stats.expenses.school.acc = std::stoi(xml_val);

    else if(xml_tag == "ly_health_cost")              world.stats.expenses.health.stat = std::stoi(xml_val);
    else if(xml_tag == "health_cost")                 world.stats.expenses.health.acc = std::stoi(xml_val);
    else if(xml_tag == "ly_deaths_cost")              world.stats.expenses.deaths.stat = std::stoi(xml_val);
    else if(xml_tag == "deaths_cost")                 world.stats.expenses.deaths.acc = std::stoi(xml_val);
    else if(xml_tag == "ly_rocket_pad_cost")          world.stats.expenses.rockets.stat = std::stoi(xml_val);
    else if(xml_tag == "rocket_pad_cost")             world.stats.expenses.rockets.acc = std::stoi(xml_val);

    else if(xml_tag == "ly_windmill_cost")            world.stats.expenses.windmill.stat = std::stoi(xml_val);
    else if(xml_tag == "windmill_cost")               world.stats.expenses.windmill.acc = std::stoi(xml_val);
    else if(xml_tag == "ly_fire_cost")                world.stats.expenses.firestation.stat = std::stoi(xml_val);
    else if(xml_tag == "fire_cost")                   world.stats.expenses.firestation.acc = std::stoi(xml_val);
    else if(xml_tag == "ly_cricket_cost")             world.stats.expenses.cricket.stat = std::stoi(xml_val);
    else if(xml_tag == "cricket_cost")                world.stats.expenses.cricket.acc = std::stoi(xml_val);

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
    else if(xml_tag == "cheat_flag")                  cheat_flag = std::stoi(xml_val);
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
          {"pollution", world.stats.history.pollution}
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
                if(xmlReader.get_name() == "capacity")
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

  modern_windmill_flag = world.tech_level > MODERN_WINDMILL_TECH;
}

static void saveMap(xmlTextWriterPtr xmlWriter, const Map& map) {
  for(int y = 0; y < map.len(); y++)
  for(int x = 0; x < map.len(); x++) {
    const MapTile& tile = *map(x, y);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"MapTile");
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"group", "%d", tile.group);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-x", "%d", x);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-y", "%d", y);
      saveMapTile(xmlWriter, tile);
    xmlTextWriterEndElement(xmlWriter);
  }

  for(int i = 0; i < map.constructionCount.size(); i++) {
    const Construction *cst = map.constructionCount.pos(i);
    if(!cst) continue;
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"Construction");
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"group", "%d", cst->constructionGroup->group);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-x", "%d", cst->x);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-y", "%d", cst->y);
      cst->save(xmlWriter);
    xmlTextWriterEndElement(xmlWriter);
  }
}

static void loadMap(xmlpp::TextReader& xmlReader, World& world) {
  Map& map = world.map;
  std::list<std::pair<Construction *, std::pair<int, int>>> constructions;

  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "map");
  int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    if(xmlReader.get_name() == "MapTile") {
      unsigned short group = std::stoi(xmlReader.get_attribute("group"));
      int x = std::stoi(xmlReader.get_attribute("map-x"));
      int y = std::stoi(xmlReader.get_attribute("map-y"));
      if(!map.is_inside(x, y))
        throw std::runtime_error("a MapTile seems to be outside the map");
      MapTile& tile = *map(x, y);
      tile.group = group;
      loadMapTile(xmlReader, tile);
    }
    else if(xmlReader.get_name() == "Construction") {
      int group = std::stoi(xmlReader.get_attribute("group"));
      int x = std::stoi(xmlReader.get_attribute("map-x"));
      int y = std::stoi(xmlReader.get_attribute("map-y"));
      ConstructionGroup *cstgrp =
        ConstructionGroup::getConstructionGroup(group);
      if(!cstgrp)
        throw std::runtime_error("invalid group");
      Construction *cst = cstgrp->createConstruction(world);
      cst->load(xmlReader);
      constructions.emplace_back(cst, std::pair(x, y));
    }
    else
      unexpectedXmlElement(xmlReader);

    xmlReader.next();
  }
  assert(xmlReader.get_name() == "map");
  assert(xmlReader.get_depth() == depth);

  for(auto& cst : constructions) {
    cst.first->place(cst.second.first, cst.second.second);
  }

  map.alt_min = map.alt_max = map(0, 0)->ground.altitude;
  for (int i = 0; i < map.len() * map.len(); i++) {
    map.alt_min = std::min(map.alt_min, map(i)->ground.altitude);
    map.alt_max = std::max(map.alt_max, map(i)->ground.altitude);
  }
  map.alt_step = (map.alt_max - map.alt_min) / 10;

  connect_transport(1, 1, map.len() - 2, map.len() - 2);
  desert_water_frontiers(0, 0, map.len(), map.len());
}

static void saveMapTile(xmlTextWriterPtr xmlWriter, MapTile& tile) {
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

static void loadMapTile(xmlpp::TextReader& xmlReader, MapTile& tile) {
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "MapTile");
  int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    assert(xmlReader.get_depth() == depth + 1);
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }

    std::string xml_tag = xmlReader.get_name();
    std::string xml_val = xmlReader.read_inner_xml();
    if     (xml_tag == "flags")      tile.flags             = std::stoul(xml_val, NULL, 0) & ~VOLATILE_FLAGS;
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
  std::function<void(xmlTextReaderPtr, const typename A::value_type&)>
    writeElement
) {
  xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"size", "%zu",
    array.size());
  for(int i = 0; i < array.size(); i++) {
    xmlTextWriterStartElement(xmlWriter, (xmlStr)std::to_string(i));
      writeElement(xmlWriter, array.at(i));
    xmlTextWriterEndElement(xmlWriter);
  }
}

template<typename A>
static void readArray(xmlpp::TextReader& xmlReader, A& array,
  std::function<void(xmlpp::TextReader&, typename A::value_type&)> readElement
) {
  array.resize(std::stoull(xmlReader.get_attribute("size")));
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  int depth = xmlReader.get_depth();
  if(!xmlReader.is_empty_element() && xmlReader.read())
  while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      xmlReader.next();
      continue;
    }
    typename A::value_type *element = nullptr;
    try {
      element = &array.at(std::stoull(xmlReader.get_name()));
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
  int diff = 0;
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "pbar");
  int depth = xmlReader.get_depth();
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
}

static void readPbar_old(xmlpp::TextReader& xmlReader,
  std::deque<Stats::Inventory<>>& array
) {
  int diff = 0;
  assert(xmlReader.get_node_type() == xmlpp::TextReader::NodeType::Element);
  assert(xmlReader.get_name() == "pbar");
  int depth = xmlReader.get_depth();
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
}
