/* ---------------------------------------------------------------------- *
 * src/lincity/world.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
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

#include "world.h"

#include <assert.h>         // for assert
#include <stdlib.h>         // for NULL, size_t
#include <iostream>         // for char_traits, basic_ostream, basic_ostream...
#include <iterator>         // for advance
#include <map>              // for map
#include <string>           // for basic_string, operator==, operator<, oper...

#include "all_buildings.h"  // for TileConstructionGroup, desertConstruction...
#include "engglobs.h"       // for world, binary_mode, dxo, dyo, world_id
#include "groups.h"         // for GROUP_WATER, GROUP_DESERT, GROUP_BARE
#include "init_game.h"      // for clear_game
#include "lin-city.h"       // for FLAG_TRANSPARENT, FLAG_IS_RIVER, FLAG_ALT...
#include "lintypes.h"       // for Construction, ConstructionGroup
#include "resources.hpp"    // for ExtraFrame, ResourceGroup, GraphicsInfo
#include "xmlloadsave.h"    // for XMLTemplate, bin_template_libary, xml_tem...


Ground::Ground() {
  altitude = 0;
  ecotable = 0;
  wastes = 0;
  pollution = 0;
  water_alt = 0;
  water_wast = 0;
  water_next = 0;
  int1 = 0;
  int2 = 0;
  int3 = 0;
  int4 = 0;
}
Ground::~Ground() {}



MapTile::MapTile():ground()
{
    construction = NULL;
    reportingConstruction = NULL;
    framesptr = NULL;
    flags = 0;
    type = 0;
    group = 0;
    pollution = 0;
    ore_reserve = 0;
    coal_reserve = 0;
}

MapTile::~MapTile()
{
    //Here the order matters
    if(construction)
    {   delete construction;}
    if (framesptr)
    {
        framesptr->clear();
        delete framesptr;
    }
}

void MapTile::setTerrain(unsigned short new_group)
{
    this->type = 0;
    this->group = new_group;
    if(new_group == GROUP_WATER)
    {   flags |= FLAG_HAS_UNDERGROUND_WATER;}
}

ConstructionGroup* MapTile::getTileConstructionGroup()
{
    switch (group)
    {
        case GROUP_BARE:    return &bareConstructionGroup;      break;
        case GROUP_DESERT:  return &desertConstructionGroup;    break;
        case GROUP_WATER:   return &waterConstructionGroup;     break;
        case GROUP_TREE:    return &treeConstructionGroup;      break;
        case GROUP_TREE2:   return &tree2ConstructionGroup;     break;
        case GROUP_TREE3:   return &tree3ConstructionGroup;     break;
        default:
            std::cout << "invalid group of maptile at: (" << world.map_x(this) <<"," << world.map_y(this) << ")" << std::endl;
            return &desertConstructionGroup;
    }
}

ResourceGroup* MapTile::getTileResourceGroup()
{
    static bool initialized = false;
    static ResourceGroup* bare = 0;
    static ResourceGroup* desert = 0;
    static ResourceGroup* water = 0;
    static ResourceGroup* tree = 0;
    static ResourceGroup* tree2 = 0;
    static ResourceGroup* tree3 = 0;

    if (!initialized)
    {
        initialized = true;
        //These must match the Group names from all_modules.cpp NOT all_buldings.cpp
        bare = ResourceGroup::resMap["Green"];
        desert = ResourceGroup::resMap["Desert"];
        water = ResourceGroup::resMap["Water"];
        tree = ResourceGroup::resMap["Tree"];
        tree2 = ResourceGroup::resMap["Tree2"];
        tree3 = ResourceGroup::resMap["Tree3"];

    }
    switch (group)
    {
        case GROUP_BARE:    return bare;      break;
        case GROUP_DESERT:  return desert;    break;
        case GROUP_WATER:   return water;     break;
        case GROUP_TREE:    return tree;      break;
        case GROUP_TREE2:   return tree2;     break;
        case GROUP_TREE3:   return tree3;     break;
        default:
            std::cout << "invalid group of maptile at: (" << world.map_x(this) <<"," << world.map_y(this) << ")" << std::endl;
            return desert;
    }
}

ConstructionGroup* MapTile::getConstructionGroup() //constructionGroup of bare land or the covering construction
{   return (reportingConstruction ? reportingConstruction->constructionGroup : getTileConstructionGroup());}

ConstructionGroup* MapTile::getTopConstructionGroup() //constructionGroup of bare land or the actual construction
{   return (construction ? construction->constructionGroup : getTileConstructionGroup());}

ConstructionGroup* MapTile::getLowerstVisibleConstructionGroup()
{
    if(!reportingConstruction || reportingConstruction->flags & FLAG_TRANSPARENT)
    {   return getTileConstructionGroup();}
    else
    {   return getConstructionGroup();}
}

unsigned short MapTile::getType() //type of bare land or the covering construction
{   return (reportingConstruction ? reportingConstruction->frameIt->frame : type);}

unsigned short MapTile::getTopType() //type of bare land or the actual construction
{   return (construction ? construction->frameIt->frame : type);}

unsigned short MapTile::getLowerstVisibleType()
{
    if(!reportingConstruction || reportingConstruction->flags & FLAG_TRANSPARENT)
    {   return type;}
    else
    {   return reportingConstruction->frameIt->frame;}
}

unsigned short MapTile::getGroup() //group of bare land or the covering construction
{   return (reportingConstruction ? reportingConstruction->constructionGroup->group : group);}

unsigned short MapTile::getTransportGroup() //group of bare land or the covering construction
{
    unsigned short grp = getGroup();
    if (is_transport())
    {
        switch(grp)
        {
            case GROUP_TRACK_BRIDGE:
                grp = GROUP_TRACK;
                break;
            case GROUP_ROAD_BRIDGE:
                grp = GROUP_ROAD;
                break;
            case GROUP_RAIL_BRIDGE:
                grp = GROUP_RAIL;
                break;
            default:
                break;
        }
    }
    return grp;
}

unsigned short MapTile::getTopGroup() //group of bare land or the actual construction
{
    if(!construction) //simple case
    {   return group;}
    else if(construction == reportingConstruction) //regular construction
    {   return construction->constructionGroup->group;}
    else // in case a construction has a chile e.g. waste burning on transport
    {   return (reportingConstruction ? reportingConstruction->constructionGroup->group : group);}
}

unsigned short MapTile::getLowerstVisibleGroup()
{
    if(!reportingConstruction || reportingConstruction->flags & FLAG_TRANSPARENT)
    {   return group;}
    else
    {   return reportingConstruction->constructionGroup->group;}
}


bool MapTile::is_bare() //true if we there is neither a covering construction nor water
{   return (!reportingConstruction) && (group != GROUP_WATER);}

bool MapTile::is_water() //true on bridges or lakes (also under bridges)
{   return (group == GROUP_WATER);}

bool MapTile::is_lake() //true on lakes (also under bridges)
{   return (group == GROUP_WATER) && !(flags & FLAG_IS_RIVER);}

bool MapTile::is_river() // true on rivers (also under bridges)
{   return (flags & FLAG_IS_RIVER);}

bool MapTile::is_visible() // true if tile is not covered by another construction. Only useful for minimap Gameview is rotated to upperleft
{   return (construction || !reportingConstruction);}

bool MapTile::is_transport() //true on tracks, road, rails and bridges
{   return (reportingConstruction && reportingConstruction->flags & FLAG_IS_TRANSPORT);}

bool MapTile::is_residence() //true on residences
{
    return (reportingConstruction &&(
        (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_LL)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_ML)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_HL)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_LH)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_MH)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_HH) ) );
}

void MapTile::writeTemplate()
{
    std::string xml_tag;
    if ((!binary_mode && xml_template_libary.count("tile") == 0)
        || (binary_mode && bin_template_libary.count(GROUP_DESERT) == 0) )
    {

        XMLTemplate * xml_tmp = new XMLTemplate("tile");
        if (!binary_mode)
        {
            xml_tmp->putTag("group");
            xml_tmp->add_len(sizeof(group));
            xml_tmp->putTag("type");
            xml_tmp->add_len(sizeof(type));
            xml_tmp->putTag("map_x");
            xml_tmp->add_len(sizeof(int));//has to match local var in saveMembers
            xml_tmp->putTag("map_y");
            xml_tmp->add_len(sizeof(int));//has to match local var in saveMembers
        }
        xml_tmp->putTag("flags");
        xml_tmp->add_len(sizeof(flags));
        xml_tmp->putTag("air_pol");
        xml_tmp->add_len(sizeof(pollution));
        xml_tmp->putTag("ore");
        xml_tmp->add_len(sizeof(ore_reserve));
        xml_tmp->putTag("coal");
        xml_tmp->add_len(sizeof(coal_reserve));
        xml_tmp->putTag("altitude");
        xml_tmp->add_len(sizeof(ground.altitude));
        xml_tmp->putTag("ecotable");
        xml_tmp->add_len(sizeof(ground.ecotable));
        xml_tmp->putTag("wastes");
        xml_tmp->add_len(sizeof(ground.wastes));
        xml_tmp->putTag("grd_pol");
        xml_tmp->add_len(sizeof(ground.pollution));
        xml_tmp->putTag("water_alt");
        xml_tmp->add_len(sizeof(ground.water_alt));
        xml_tmp->putTag("water_pol");
        xml_tmp->add_len(sizeof(ground.water_pol));
        xml_tmp->putTag("water_next");
        xml_tmp->add_len(sizeof(ground.water_next));
        xml_tmp->putTag("int1");
        xml_tmp->add_len(sizeof(ground.int1));
        xml_tmp->putTag("int2");
        xml_tmp->add_len(sizeof(ground.int2));
        xml_tmp->putTag("int3");
        xml_tmp->add_len(sizeof(ground.int3));
        xml_tmp->putTag("int4");
        xml_tmp->add_len(sizeof(ground.int4));
        if (binary_mode)
        {   xml_tmp->set_group(GROUP_DESERT);}
    }
}


void MapTile::saveMembers(std::ostream *os)
{
    type = type % getTileResourceGroup()->graphicsInfoVector.size();
    int x = world.map_x(this);
    int y = world.map_y(this);
    unsigned short head = GROUP_DESERT;
    size_t cm = 0;
    std::string xml_tag;
     if ((!binary_mode && xml_template_libary.count("tile") == 0)
        || (binary_mode && bin_template_libary.count(head) == 0) )
    {
        writeTemplate();
        //std::cout << "creating xml template for tile" << std::endl;
    }
    XMLTemplate * xml_tmp;
    if (binary_mode)
    {   xml_tmp = bin_template_libary[head];}
    else
    {
        xml_tmp = xml_template_libary["tile"];
        if (os == &std::cout)
        {   xml_tmp->report(os);}
    }
    xml_tmp->rewind();

    if (!binary_mode)
    {
        *os << "<tile>";
        os->flush();
        while (!xml_tmp->reached_end())
        {
            xml_tag = xml_tmp->getTag();
            if (xml_tag == "map_x")                  {*os << x;}
            else if (xml_tag == "map_y")             {*os << y;}
            //mapTile
            else if (xml_tag == "flags")             {*os << flags;}
            else if (xml_tag == "type")              {*os << type;}
            else if (xml_tag == "group")             {*os << group;}
            else if (xml_tag == "air_pol")           {*os << pollution;}
            else if (xml_tag == "ore")               {*os << ore_reserve;}
            else if (xml_tag == "coal")              {*os << coal_reserve;}
            //ground
            else if (xml_tag == "altitude")          {*os << ground.altitude;}
            else if (xml_tag == "ecotable")          {*os << ground.ecotable;}
            else if (xml_tag == "wastes")            {*os << ground.wastes;}
            else if (xml_tag == "grd_pol")           {*os << ground.pollution;}
            else if (xml_tag == "water_alt")         {*os << ground.water_alt;}
            else if (xml_tag == "water_pol")         {*os << ground.water_pol;}
            else if (xml_tag == "water_wast")        {*os << ground.water_wast;}
            else if (xml_tag == "water_next")        {*os << ground.water_next;}
            else if (xml_tag == "int1")              {*os << ground.int1;}
            else if (xml_tag == "int2")              {*os << ground.int2;}
            else if (xml_tag == "int3")              {*os << ground.int3;}
            else if (xml_tag == "int4")              {*os << ground.int4;}
            else
            {
                std::cout<<"Unknown Template entry "<< xml_tag << " while exporting <MapTile>"<<std::endl;
            }

            *os << '\t';
            os->flush();
            xml_tmp->step();
        }
        *os << "</tile>" << std::endl;
    }
    else
    {
        int idx = x + y * world.len();
        os->write( (char*)&head,sizeof(head)); //Head aka GROUP_DESERT
        os->write( (char*)&group,sizeof(group));
        os->write( (char*)&type,sizeof(type));
        os->write( (char*)&idx,sizeof(idx));

        while (!xml_tmp->reached_end())
        {
            xml_tag = xml_tmp->getTag();
            xml_tmp->step();
            //mapTile
            if (xml_tag == "flags")                  {os->write( (char*)&flags,sizeof(flags));cm+=sizeof(flags);}
            else if (xml_tag == "air_pol")           {os->write( (char*)&pollution,sizeof(pollution));cm+=sizeof(pollution);}
            else if (xml_tag == "ore")               {os->write( (char*)&ore_reserve,sizeof(ore_reserve));cm+=sizeof(ore_reserve);}
            else if (xml_tag == "coal")              {os->write( (char*)&coal_reserve,sizeof(coal_reserve));cm+=sizeof(coal_reserve);}
            //ground
            else if (xml_tag == "altitude")          {os->write( (char*)&ground.altitude,sizeof(ground.altitude));cm+=sizeof(ground.altitude);}
            else if (xml_tag == "ecotable")          {os->write( (char*)&ground.ecotable,sizeof(ground.ecotable));cm+=sizeof(ground.ecotable);}
            else if (xml_tag == "wastes")            {os->write( (char*)&ground.wastes,sizeof(ground.wastes));cm+=sizeof(ground.wastes);}
            else if (xml_tag == "grd_pol")           {os->write( (char*)&ground.pollution,sizeof(ground.pollution));cm+=sizeof(ground.pollution);}
            else if (xml_tag == "water_alt")         {os->write( (char*)&ground.water_alt,sizeof(ground.water_alt));cm+=sizeof(ground.water_alt);}
            else if (xml_tag == "water_pol")         {os->write( (char*)&ground.water_pol,sizeof(ground.water_pol));cm+=sizeof(ground.water_pol);}
            else if (xml_tag == "water_wast")        {os->write( (char*)&ground.water_wast,sizeof(ground.water_wast));cm+=sizeof(ground.water_wast);}
            else if (xml_tag == "water_next")        {os->write( (char*)&ground.water_next,sizeof(ground.water_next));cm+=sizeof(ground.water_next);}
            else if (xml_tag == "int1")              {os->write( (char*)&ground.int1,sizeof(ground.int1));cm+=sizeof(ground.int1);}
            else if (xml_tag == "int2")              {os->write( (char*)&ground.int2,sizeof(ground.int2));cm+=sizeof(ground.int2);}
            else if (xml_tag == "int3")              {os->write( (char*)&ground.int3,sizeof(ground.int3));cm+=sizeof(ground.int3);}
            else if (xml_tag == "int4")              {os->write( (char*)&ground.int4,sizeof(ground.int4));cm+=sizeof(ground.int4);}
            else
            {
                std::cout<<"Unknown Template entry "<< xml_tag << " while exporting <MapTile>"<<std::endl;
            }
        }
        //std::cout << "cm/len: " << cm << "/" << xml_tmp->len() << std::endl;
        assert(cm == xml_tmp->len());
    }
}

std::list<ExtraFrame>::iterator MapTile::createframe(void)
{
    if(!framesptr)
    {   framesptr = new std::list<ExtraFrame>;}
    framesptr->resize(framesptr->size() + 1);
    std::list<ExtraFrame>::iterator frit = framesptr->end();
    std::advance(frit, -1);
    return frit; //the last position
}

void MapTile::killframe(std::list<ExtraFrame>::iterator it)
{
    //what would actually happen if "it" belongs to another maptile?
    framesptr->erase(it);
    if (framesptr->empty())
    {
        delete framesptr;
        framesptr = NULL;
    }
}



World::World(int map_len)
{
    maptile.resize(map_len * map_len);
    dirty = false;
    world.climate = -1;
    world.old_setup_ground = -1;
    //std::cout << "created World len = " << len() << "²" << std::endl;
}

World::~World()
{
    maptile.clear();
}

void World::len(int new_len)
{
    if (new_len < 50)
    {   new_len = 50;}
    if (dirty) {clear_game();}
    bool job_done = false;

    while (!job_done)
    {
        try
        {
            this->side_len = new_len;
            job_done = true;
            maptile.resize(new_len * new_len);
        }
        catch(...)
        {
            new_len -= 25;
            std::cout << "failed to allocate world. shrinking edge to " << new_len << " tiles" << std::endl;
            job_done = false;
            if (new_len < 50) //Ok we give up, but should crash very soon anyways.
            {   return;}
        }
    }
}

MapTile* World::operator()(int x, int y)
{
    return &(maptile[x + y * side_len]);
}

MapTile* World::operator()(int index)
{
    return &(maptile[index]);
}

bool World::is_inside(int x, int y)
{
    return (x >= 0 && y >= 0 && x < side_len && y < side_len);
}

bool World::is_inside(int index)
{
    return (index >= 0 && index < side_len * side_len);
}

bool World::is_border(int x, int y)
{
    return (x == 0 || y == 0 || x == side_len-1 || y == side_len -1);
}

bool World::is_border(int index)
{
    return (index%side_len == side_len -1 || index%side_len == 0 || index/side_len == side_len-1 || index/side_len == 0);
}

bool World::is_edge(int x, int y)
{
    return (x == 1 || y == 1 || x == side_len-2 || y == side_len -2);
}

bool World::is_visible(int x, int y)
{
    return (x > 0 && y > 0 && x < side_len-1 && y < side_len -1);
}

int World::map_x(MapTile * tile)
{
    return (tile-&maptile[0]) % side_len;
}

int World::map_y(MapTile * tile)
{
    return (tile-&maptile[0]) / side_len;
}

int World::map_index(MapTile * tile)
{
    return (tile-&maptile[0]);
}

int World::len()
{
    return side_len;
}

int World::seed()
{
#ifdef DEBUG
    assert(world_id == id);
#endif
    return id;
}

void World::seed( int new_seed)
{
    this->id = new_seed;
    world_id = new_seed;
}

bool World::maximum(int x , int y)
{
    int alt = maptile[x + y * side_len].ground.altitude;
    bool is_max = true;
    for (int i=0; i<8; i++)
    {
        int tx = x + dxo[i];
        int ty = y + dyo[i];
        is_max &= (alt >= maptile[tx + ty * side_len].ground.altitude);
    }
    return is_max;
}

bool World::minimum(int x , int y)
{
    int alt = maptile[x + y * side_len].ground.altitude;
    bool is_min = true;
    for (int i=0; i<8; i++)
    {
        int tx = x + dxo[i];
        int ty = y + dyo[i];
        is_min &= (alt <= maptile[tx + ty * side_len].ground.altitude);
    }
    return is_min;
}

bool World::saddlepoint(int x , int y)
{
    int alt = maptile[x + y * side_len].ground.altitude;
    int dips = 0;
    bool dip_new = alt > maptile[x + dxo[7] + (y + dyo[7])*side_len ].ground.altitude;
    bool dip_old = dip_new;
    for (int i=0; i<8; i++)
    {
        dip_new = alt > maptile[x + dxo[i]+ (y + dyo[i])*side_len].ground.altitude;
        if (dip_new && !dip_old) //We just stepped into a valley
        {
                dips++;
        }
        dip_old = dip_new;
    }
    return dips > 1;
}

bool World::checkEdgeMin(int x , int y)
{
    int alt = maptile[x + y * side_len].ground.altitude;
    if (x==1 || x == side_len-2)
    {
        return alt < maptile[x+1 + y * side_len].ground.altitude
            && alt < maptile[x-1 + y * side_len].ground.altitude;
    }
    else if (y==1 || y == side_len-2)
    {
        return alt < maptile[x + (y+1) * side_len].ground.altitude
            && alt < maptile[x + (y-1) * side_len].ground.altitude;
    }
    else
        return false;
}

int World::count_altered(void)
{
    int count = 0;
    for (size_t i = 0; i < maptile.size(); ++i)
    {
        if(maptile[i].flags & FLAG_ALTERED)
        {   ++count;}
    }
    return count;
}



/** @file lincity/world.cpp */
