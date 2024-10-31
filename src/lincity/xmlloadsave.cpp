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
#include <libxml/xmlreader.h>              // for xmlReaderForIO, xmlTextRea...
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
#include "world.h"                         // for MapTile, Ground, World

static void saveGlobals(xmlTextWriterPtr xmlWriter);
static void loadGlobals(xmlpp::TextReader& xmlReader);
static void saveMap(xmlTextWriterPtr xmlWriter);
static void loadMap(xmlpp::TextReader& xmlReader);
static void saveMapTile(xmlTextWriterPtr xmlWriter, MapTile& tile);
static void loadMapTile(xmlpp::TextReader& xmlReader, MapTile& tile);
static void writeArray(xmlTextWriterPtr xmlWriter, int *ary, int len);
static void readArray(xmlpp::TextReader& xmlReader, int *ary, int len);
static void writePbar(xmlTextWriterPtr xmlWriter, struct pbar_st& pbar);
static void readPbar(xmlpp::TextReader& xmlReader, struct pbar_st& pbar);

void saveGame(std::string filename) {
  std::string gz_name;
  gzFile gz_file = gzopen(filename.c_str(), "wb");
  if(!gz_file)
    throw std::runtime_error(
      std::string("failed to open file: ") + filename);

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
    [](xmlTextWriterPtr xmlWriter) {
      xmlTextWriterClose(xmlWriter);
      xmlFreeTextWriter(xmlWriter);
    }
  );

#ifdef DEBUG
  xmlTextWriterSetIndent(xmlWriter, true);
  xmlTextWriterSetIndentString(xmlWriter, (xmlStr)"  ");
#else
  xmlTextWriterSetIndent(xmlWriter, false);
#endif

  xmlTextWriterStartDocument(xmlWriter, NULL, NULL, NULL);
  xmlTextWriterWriteComment(xmlWriter, (xmlStr)"This file is a lincity savegame.");
  xmlTextWriterStartElement(xmlWriter, (xmlStr)"lc-game");
    xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"ldsv-version", "%u",
      LOADSAVE_VERSION_CURRENT);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"globals");
      saveGlobals(xmlWriter);
    xmlTextWriterEndElement(xmlWriter);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"map");
      saveMap(xmlWriter);
    xmlTextWriterEndElement(xmlWriter);
  xmlTextWriterEndElement(xmlWriter);
  xmlTextWriterEndDocument(xmlWriter);
}

void loadGame(std::string filename) {
  gzFile gz_file = gzopen(filename.c_str(), "rb");
  if(!gz_file)
    throw std::runtime_error(
      std::string("failed to open file: ") + filename);

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
      throw std::runtime_error("failed to find SaveGame element");
    else if(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element);
    else if(xmlReader.get_name() == "lc-game")
      break;
    else
      unexpectedXmlElement(xmlReader);
    xmlReader.next();
  }

  // parse load/save version
  std::string versionStr = xmlReader.get_attribute("ldsv-version");
  if(versionStr.empty())
    throw std::runtime_error("failed to parse load/save version");
  ldsv_version = std::stoi(versionStr);
  if(ldsv_version > LOADSAVE_VERSION_CURRENT)
    throw std::runtime_error("load/save version too high");

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
      loadGlobals(xmlReader);
    }
    else if(xmlReader.get_name() == "map") {
      loadMap(xmlReader);
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
}

void unexpectedXmlElement(xmlpp::TextReader& xmlReader) {
  std::cerr << "warning: skipping unexpected element '"
    << xmlReader.get_name() << "'" << std::endl;
}

static void saveGlobals(xmlTextWriterPtr xmlWriter) {

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"given_scene",                 "%s", given_scene);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"world_id",                    "%d", world_id);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"climate",                     "%d", world.climate);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"old_setup_ground",            "%d", world.old_setup_ground);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"global_aridity",              "%d", global_aridity);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"global_mountainity",          "%d", global_mountainity);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"world_side_len",              "%d", world.len());
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"main_screen_originx",         "%d", main_screen_originx);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"main_screen_originy",         "%d", main_screen_originy);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_time",                  "%d", total_time);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"people_pool",                 "%d", people_pool);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_money",                 "%d", total_money);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_income_tax",               "%d", ly_income_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"income_tax",                  "%d", income_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"income_tax_rate",             "%d", income_tax_rate);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_interest",                 "%d", ly_interest);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_coal_tax",                 "%d", ly_coal_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal_tax",                    "%d", coal_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal_tax_rate",               "%d", coal_tax_rate);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_unemployment_cost",        "%d", ly_unemployment_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unemployment_cost",           "%d", unemployment_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"dole_rate",                   "%d", dole_rate);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_transport_cost",           "%d", ly_transport_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"transport_cost",              "%d", transport_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"transport_cost_rate",         "%d", transport_cost_rate);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_goods_tax",                "%d", ly_goods_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"goods_tax",                   "%d", goods_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"goods_tax_rate",              "%d", goods_tax_rate);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_export_tax",               "%d", ly_export_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"export_tax",                  "%d", export_tax);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"export_tax_rate",             "%d", export_tax_rate);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_import_cost",              "%d", ly_import_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"import_cost",                 "%d", import_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"import_cost_rate",            "%d", import_cost_rate);

  for(Commodity c = STUFF_INIT; c < STUFF_COUNT; c++) {
    const char * const &cname = commodityNames[c];
    xmlTextWriterWriteFormatElement(xmlWriter,
      (xmlStr)(std::string("import_")+cname+"_enable").c_str(), "%d",
      portConstructionGroup.tradeRule[c].take);
    xmlTextWriterWriteFormatElement(xmlWriter,
      (xmlStr)(std::string("export_")+cname+"_enable").c_str(), "%d",
      portConstructionGroup.tradeRule[c].give);
  }

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_university_cost",          "%d", ly_university_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"university_cost",             "%d", university_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_recycle_cost",             "%d", ly_recycle_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"recycle_cost",                "%d", recycle_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_school_cost",              "%d", ly_school_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"school_cost",                 "%d", school_cost);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_health_cost",              "%d", ly_health_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"health_cost",                 "%d", health_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_deaths_cost",              "%d", ly_deaths_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"deaths_cost",                 "%d", deaths_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_rocket_pad_cost",          "%d", ly_rocket_pad_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"rocket_pad_cost",             "%d", rocket_pad_cost);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_windmill_cost",            "%d", ly_windmill_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"windmill_cost",               "%d", windmill_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_fire_cost",                "%d", ly_fire_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"fire_cost",                   "%d", fire_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"ly_cricket_cost",             "%d", ly_cricket_cost);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"cricket_cost",                "%d", cricket_cost);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tech_level",                  "%d", tech_level);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"highest_tech_level",          "%d", highest_tech_level);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tpopulation",                 "%d", tpopulation);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"thousing",                    "%d", thousing);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tstarving_population",        "%d", tstarving_population);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"tunemployed_population",      "%d", tunemployed_population);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_pollution",             "%d", total_pollution);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"rockets_launched",            "%d", rockets_launched);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"rockets_launched_success",    "%d", rockets_launched_success);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"coal_survey_done",            "%d", coal_survey_done);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"cheat_flag",                  "%d", cheat_flag);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_pollution_deaths",      "%d", total_pollution_deaths);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"pollution_deaths_history",    "%f", pollution_deaths_history);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_starve_deaths",         "%d", total_starve_deaths);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"starve_deaths_history",       "%f", starve_deaths_history);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_unemployed_years",      "%d", coal_survey_done);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"unemployed_history",          "%f", unemployed_history);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"max_pop_ever",                "%d", max_pop_ever);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_evacuated",             "%d", total_evacuated);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"total_births",                "%d", total_births);

  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_dig_ore_coal_tip_flag",  "%d", sust_dig_ore_coal_tip_flag);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_dig_ore_coal_count",     "%d", sust_dig_ore_coal_count);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_port_count",             "%d", sust_port_count);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_money_count",        "%d", sust_old_money_count);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_population_count",   "%d", sust_old_population_count);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_tech_count",         "%d", sust_old_tech_count);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_fire_count",             "%d", sust_fire_count);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_money",              "%d", sust_old_money);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_port_flag",              "%d", sust_port_flag);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_population",         "%d", sust_old_population);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sust_old_tech",               "%d", sust_old_tech);
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"sustain_flag",                "%d", sustain_flag);

  xmlTextWriterStartElement(xmlWriter, (xmlStr)"monthgraph_pop");
    writeArray(xmlWriter, monthgraph_pop, monthgraph_size);
  xmlTextWriterEndElement(xmlWriter);
  xmlTextWriterStartElement(xmlWriter, (xmlStr)"monthgraph_starve");
    writeArray(xmlWriter, monthgraph_starve, monthgraph_size);
  xmlTextWriterEndElement(xmlWriter);
  xmlTextWriterStartElement(xmlWriter, (xmlStr)"monthgraph_nojobs");
    writeArray(xmlWriter, monthgraph_nojobs, monthgraph_size);
  xmlTextWriterEndElement(xmlWriter);
  xmlTextWriterStartElement(xmlWriter, (xmlStr)"monthgraph_ppool");
    writeArray(xmlWriter, monthgraph_ppool, monthgraph_size);
  xmlTextWriterEndElement(xmlWriter);

  for(int pbarId = 0; pbarId < NUM_PBARS; pbarId++) {
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"pbar");
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"id", "%d", pbarId);
      writePbar(xmlWriter, pbars[pbarId]);
    xmlTextWriterEndElement(xmlWriter);
  }
}

static void loadGlobals(xmlpp::TextReader& xmlReader) {
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

    std::string xml_tag = xmlReader.get_name();
    std::string xml_val = xmlReader.read_inner_xml();
    if(xml_tag == "given_scene")                      std::strncpy(given_scene, xml_val.c_str(), 1024);
    else if(xml_tag == "global_aridity")              global_aridity = std::stoi(xml_val);
    else if(xml_tag == "global_mountainity")          global_mountainity = std::stoi(xml_val);
    else if(xml_tag == "world_side_len")              world.len(std::stoi(xml_val));
    else if(xml_tag == "world_id")                    world.seed(std::stoi(xml_val));
    else if(xml_tag == "old_setup_ground")            world.old_setup_ground = std::stoi(xml_val);
    else if(xml_tag == "climate")                     world.climate = std::stoi(xml_val);

    else if(xml_tag == "main_screen_originx")         main_screen_originx = std::stoi(xml_val);
    else if(xml_tag == "main_screen_originy")         main_screen_originy = std::stoi(xml_val);
    else if(xml_tag == "total_time")                  total_time = std::stoi(xml_val);

    else if(xml_tag == "people_pool")                 people_pool = std::stoi(xml_val);
    else if(xml_tag == "total_money")                 total_money = std::stoi(xml_val);
    else if(xml_tag == "ly_income_tax")               ly_income_tax = std::stoi(xml_val);
    else if(xml_tag == "income_tax")                  income_tax = std::stoi(xml_val);
    else if(xml_tag == "income_tax_rate")             income_tax_rate = std::stoi(xml_val);

    else if(xml_tag == "ly_interest")                 ly_interest = std::stoi(xml_val);
    else if(xml_tag == "ly_coal_tax")                 ly_coal_tax = std::stoi(xml_val);
    else if(xml_tag == "coal_tax")                    coal_tax = std::stoi(xml_val);
    else if(xml_tag == "coal_tax_rate")               coal_tax_rate = std::stoi(xml_val);

    else if(xml_tag == "ly_unemployment_cost")        ly_unemployment_cost = std::stoi(xml_val);
    else if(xml_tag == "unemployment_cost")           unemployment_cost = std::stoi(xml_val);
    else if(xml_tag == "dole_rate")                   dole_rate = std::stoi(xml_val);

    else if(xml_tag == "ly_transport_cost")           ly_transport_cost = std::stoi(xml_val);
    else if(xml_tag == "transport_cost")              transport_cost = std::stoi(xml_val);
    else if(xml_tag == "transport_cost_rate")         transport_cost_rate = std::stoi(xml_val);
    else if(xml_tag == "ly_goods_tax")                ly_goods_tax = std::stoi(xml_val);
    else if(xml_tag == "goods_tax")                   goods_tax = std::stoi(xml_val);
    else if(xml_tag == "goods_tax_rate")              goods_tax_rate = std::stoi(xml_val);

    else if(xml_tag == "ly_export_tax")               ly_export_tax = std::stoi(xml_val);
    else if(xml_tag == "export_tax")                  export_tax = std::stoi(xml_val);
    else if(xml_tag == "export_tax_rate")             export_tax_rate = std::stoi(xml_val);
    else if(xml_tag == "ly_import_cost")              ly_import_cost = std::stoi(xml_val);
    else if(xml_tag == "import_cost")                 import_cost = std::stoi(xml_val);
    else if(xml_tag == "import_cost_rate")            import_cost_rate = std::stoi(xml_val);

    else if(xml_tag == "ly_university_cost")          ly_university_cost = std::stoi(xml_val);
    else if(xml_tag == "university_cost")             university_cost = std::stoi(xml_val);
    else if(xml_tag == "ly_recycle_cost")             ly_recycle_cost = std::stoi(xml_val);
    else if(xml_tag == "recycle_cost")                recycle_cost = std::stoi(xml_val);
    else if(xml_tag == "ly_school_cost")              ly_school_cost = std::stoi(xml_val);
    else if(xml_tag == "school_cost")                 school_cost = std::stoi(xml_val);

    else if(xml_tag == "ly_health_cost")              ly_health_cost = std::stoi(xml_val);
    else if(xml_tag == "health_cost")                 health_cost = std::stoi(xml_val);
    else if(xml_tag == "ly_deaths_cost")              ly_deaths_cost = std::stoi(xml_val);
    else if(xml_tag == "deaths_cost")                 deaths_cost = std::stoi(xml_val);
    else if(xml_tag == "ly_rocket_pad_cost")          ly_rocket_pad_cost = std::stoi(xml_val);
    else if(xml_tag == "rocket_pad_cost")             rocket_pad_cost = std::stoi(xml_val);

    else if(xml_tag == "ly_windmill_cost")            ly_windmill_cost = std::stoi(xml_val);
    else if(xml_tag == "windmill_cost")               windmill_cost = std::stoi(xml_val);
    else if(xml_tag == "ly_fire_cost")                ly_fire_cost = std::stoi(xml_val);
    else if(xml_tag == "fire_cost")                   fire_cost = std::stoi(xml_val);
    else if(xml_tag == "ly_cricket_cost")             ly_cricket_cost = std::stoi(xml_val);
    else if(xml_tag == "cricket_cost")                cricket_cost = std::stoi(xml_val);

    else if(xml_tag == "tech_level")                  tech_level = std::stoi(xml_val);
    else if(xml_tag == "highest_tech_level")          highest_tech_level = std::stoi(xml_val);
    else if(xml_tag == "tpopulation")                 tpopulation = std::stoi(xml_val);
    else if(xml_tag == "thousing")                    thousing = std::stoi(xml_val);
    else if(xml_tag == "tstarving_population")        tstarving_population = std::stoi(xml_val);
    else if(xml_tag == "tunemployed_population")      tunemployed_population = std::stoi(xml_val);

    else if(xml_tag == "total_pollution")             total_pollution = std::stoi(xml_val);
    else if(xml_tag == "rockets_launched")            rockets_launched = std::stoi(xml_val);
    else if(xml_tag == "rockets_launched_success")    rockets_launched_success = std::stoi(xml_val);

    else if(xml_tag == "coal_survey_done")            coal_survey_done = std::stoi(xml_val);
    else if(xml_tag == "cheat_flag")                  cheat_flag = std::stoi(xml_val);
    else if(xml_tag == "total_pollution_deaths")      total_pollution_deaths = std::stoi(xml_val);
    else if(xml_tag == "pollution_deaths_history")    pollution_deaths_history = std::stof(xml_val);
    else if(xml_tag == "total_starve_deaths")         total_starve_deaths = std::stoi(xml_val);
    else if(xml_tag == "starve_deaths_history")       starve_deaths_history = std::stof(xml_val);

    else if(xml_tag == "total_unemployed_years")      total_unemployed_years = std::stoi(xml_val);
    else if(xml_tag == "unemployed_history")          unemployed_history = std::stof(xml_val);
    else if(xml_tag == "max_pop_ever")                max_pop_ever = std::stoi(xml_val);
    else if(xml_tag == "total_evacuated")             total_evacuated = std::stoi(xml_val);
    else if(xml_tag == "total_births")                total_births = std::stoi(xml_val);

    else if(xml_tag == "sust_dig_ore_coal_tip_flag")  sust_dig_ore_coal_tip_flag = std::stoi(xml_val);
    else if(xml_tag == "sust_dig_ore_coal_count")     sust_dig_ore_coal_count = std::stoi(xml_val);
    else if(xml_tag == "sust_port_count")             sust_port_count = std::stoi(xml_val);
    else if(xml_tag == "sust_old_money_count")        sust_old_money_count = std::stoi(xml_val);
    else if(xml_tag == "sust_old_population_count")   sust_old_population_count = std::stoi(xml_val);
    else if(xml_tag == "sust_old_tech_count")         sust_old_tech_count = std::stoi(xml_val);
    else if(xml_tag == "sust_fire_count")             sust_fire_count = std::stoi(xml_val);
    else if(xml_tag == "sust_old_money")              sust_old_money = std::stoi(xml_val);
    else if(xml_tag == "sust_port_flag")              sust_port_flag = std::stoi(xml_val);
    else if(xml_tag == "sust_old_population")         sust_old_population = std::stoi(xml_val);
    else if(xml_tag == "sust_old_tech")               sust_old_tech = std::stoi(xml_val);
    else if(xml_tag == "sustain_flag")                sustain_flag = std::stoi(xml_val);

    else if(xml_tag == "monthgraph_pop")              readArray(xmlReader, monthgraph_pop, monthgraph_size);
    else if(xml_tag == "monthgraph_starve")           readArray(xmlReader, monthgraph_starve, monthgraph_size);
    else if(xml_tag == "monthgraph_nojobs")           readArray(xmlReader, monthgraph_nojobs, monthgraph_size);
    else if(xml_tag == "monthgraph_ppool")            readArray(xmlReader, monthgraph_ppool, monthgraph_size);
    else if(xml_tag == "pbar") {
      int id = std::stoi(xmlReader.get_attribute("id"));
      if(id >= 0 && id < NUM_PBARS) {
        readPbar(xmlReader, pbars[id]);
      }
      else {
        std::cerr << "warning: skipping invalid pbar id: " << id << "\n"
          << "  Someone may be trying something nasty."
          << " See https://github.com/lincity-ng/lincity-ng/issues/205"
          << " for more information.\n";
      }
    }
    else goto more_globals; goto found_global; more_globals:

    for(Commodity c = STUFF_INIT; c < STUFF_COUNT; c++) {
      bool *ixenable = NULL;
      const char * const &cname = commodityNames[c];
      if(xml_tag == std::string("import_") + cname + "_enable")
        ixenable = &portConstructionGroup.tradeRule[c].take;
      else if(xml_tag == std::string("export_") + cname + "_enable")
        ixenable = &portConstructionGroup.tradeRule[c].give;
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

  ly_other_cost = ly_university_cost + ly_recycle_cost + ly_deaths_cost
    + ly_health_cost + ly_rocket_pad_cost + ly_school_cost + ly_interest
    + ly_windmill_cost + ly_fire_cost + ly_cricket_cost;
  housed_population = tpopulation / ((total_time % NUMOF_DAYS_IN_MONTH) + 1);
  modern_windmill_flag = tech_level > MODERN_WINDMILL_TECH;

  assert(main_screen_originx >= 0 && main_screen_originx < world.len());
  assert(main_screen_originy >= 0 && main_screen_originy < world.len());
}

static void saveMap(xmlTextWriterPtr xmlWriter) {
  for(int y = 0; y < world.len(); y++)
  for(int x = 0; x < world.len(); x++) {
    MapTile& tile = *world(x, y);
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"MapTile");
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"group", "%d", tile.group);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-x", "%d", x);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-y", "%d", y);
      saveMapTile(xmlWriter, tile);
    xmlTextWriterEndElement(xmlWriter);
  }

  for(int i = 0; i < ::constructionCount.size(); i++) {
    Construction *cst = ::constructionCount.pos(i);
    if(!cst) continue;
    xmlTextWriterStartElement(xmlWriter, (xmlStr)"Construction");
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"group", "%d", cst->constructionGroup->group);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-x", "%d", cst->x);
      xmlTextWriterWriteFormatAttribute(xmlWriter, (xmlStr)"map-y", "%d", cst->y);
      cst->save(xmlWriter);
    xmlTextWriterEndElement(xmlWriter);
  }
}

static void loadMap(xmlpp::TextReader& xmlReader) {
  std::list<std::pair<Construction *, std::pair<int, int>>> constructions =
    std::list<std::pair<Construction *, std::pair<int, int>>>();

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
      if(!world.is_inside(x, y))
        throw std::runtime_error("a MapTile seems to be outside the map");
      MapTile& tile = *world(x, y);
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
      Construction *cst = cstgrp->createConstruction();
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

  alt_min = alt_max = world(0)->ground.altitude;
  for (int i = 0; i < world.len() * world.len(); i++) {
    alt_min = std::min(alt_min, world(i)->ground.altitude);
    alt_max = std::max(alt_min, world(i)->ground.altitude);
  }
  alt_step = (alt_max - alt_min) / 10;

  connect_transport(1, 1, world.len() - 2, world.len() - 2);
  desert_water_frontiers(0, 0, world.len(), world.len());
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

static void writeArray(xmlTextWriterPtr xmlWriter, int *ary, int len) {
  std::ostringstream str;
  while(len-- > 0)
    str << *(ary++) << "\t";
  xmlTextWriterWriteString(xmlWriter, (xmlStr)str.str().c_str());
}

static void readArray(xmlpp::TextReader& xmlReader, int *ary, int len) {
  std::string str = xmlReader.read_inner_xml();
#if 0 // deprecated
  if(str.find("<int>") != str.npos) {
    xmlReader.read()
    while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::Element)
      xmlReader.next();
    assert(xmlReader.get_name() == "int");
    str = xmlReader.read_inner_xml();
    while(xmlReader.get_node_type() != xmlpp::TextReader::NodeType::EndElement)
      xmlReader.next();
  }
#endif
  while(len-- > 0) {
    str.erase(0, str.find_first_not_of(" \f\n\r\t\v"));
    *(ary++) = str.empty() ? 0 : std::stoi(str);
  }
}

static void writePbar(xmlTextWriterPtr xmlWriter, struct pbar_st& pbar) {
  xmlTextWriterWriteFormatElement(xmlWriter, (xmlStr)"diff", "%d", pbar.diff);
  xmlTextWriterStartElement(xmlWriter, (xmlStr)"data");
    writeArray(xmlWriter, pbar.data, PBAR_DATA_SIZE);
  xmlTextWriterEndElement(xmlWriter);
}

static void readPbar(xmlpp::TextReader& xmlReader, struct pbar_st& pbar) {
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
      pbar.diff = std::stoi(xmlReader.read_inner_xml());
    else if(xmlReader.get_name() == "data")
      readArray(xmlReader, pbar.data, PBAR_DATA_SIZE);
    else
      unexpectedXmlElement(xmlReader);

    xmlReader.next();
  }
  assert(xmlReader.get_name() == "pbar");
  assert(xmlReader.get_depth() == depth);
}
