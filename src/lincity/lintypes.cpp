/* ---------------------------------------------------------------------- *
 * lintypes.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#include "lintypes.h"
#include "ConstructionManager.h"
#include "ConstructionCount.h"

#include <string.h>             /* XXX: portability issue?  for strcpy */
#include "lcconfig.h"
#include "lin-city.h"
#include "engine.h"
#include "engglobs.h"
#include "lctypes.h"
#include "tinygettext/gettext.hpp"
#include "fileutil.h"
#include "gui_interface/readpng.h"
#include "loadsave.h"
#include "xmlloadsave.h"
#include "all_buildings.h"
#include "transport.h"
#include "modules/all_modules.h"
#include <iostream>
#include "lincity-ng/Sound.hpp"

//Ground Declarations

Ground::Ground()
{
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

//MapTile Declarations

MapTile::MapTile():ground()
{
    construction = NULL;
    reportingConstruction = NULL;
    flags = 0;
    type = 0;
    group = 0;
    pollution = 0;
    ore_reserve = 0;
    coal_reserve = 0;
}

MapTile::~MapTile()
{   }

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

ConstructionGroup* MapTile::getConstructionGroup() //constructionGroup of bare land or the covering construction
{   return (reportingConstruction ? reportingConstruction->constructionGroup : getTileConstructionGroup());}

ConstructionGroup* MapTile::getTopConstructionGroup() //constructionGroup of bare land or the actual construction
{   return (construction ? construction->constructionGroup : getTileConstructionGroup());}

unsigned short MapTile::getType() //type of bare land or the covering construction
{   return (reportingConstruction ? reportingConstruction->type : type);}

unsigned short MapTile::getTopType() //type of bare land or the actual construction
{   return (construction ? construction->type : type);}

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
{   return (construction ? construction->constructionGroup->group : group);}

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

bool MapTile::is_powerline() //true on powerlines
{   return (reportingConstruction && reportingConstruction->flags & FLAG_POWER_LINE);}

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
    type = type % getTileConstructionGroup()->graphicsInfoVector.size();
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


//Construction Declarations

const char *commodityNames[] =
    {
    "Food",
    "Jobs",
    "Coal",
    "Goods",
    "Ore",
    "Steel",
    "Waste",
    "kWh",
    "MWh",
    "Water",
    "Unknown",
    "Unknown",
    "Unknown",
    };

void Construction::list_commodities(int * i)
{
    /*
        Lists all current commodities of a construction in MPS area
        Groups commodities by incomming, outgoing, twoway and inactive
    */

    std::map<Construction::Commodities, int>::iterator stuff_it;
    if (! (flags & FLAG_EVACUATE))
    {
        for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++)
        {
            if(constructionGroup->commodityRuleCount[stuff_it->first].take
            && ! constructionGroup->commodityRuleCount[stuff_it->first].give
            && *i < 14)
            {
                mps_store_ssddp(*i,"--> ",commodityNames[stuff_it->first],stuff_it->second, constructionGroup->commodityRuleCount[stuff_it->first].maxload);
                ++*i;
            }//endif
        } //endfor
        for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++)
        {
            if(constructionGroup->commodityRuleCount[stuff_it->first].give
            && ! constructionGroup->commodityRuleCount[stuff_it->first].take
            && *i<14)
            {
                mps_store_ssddp(*i,"<-- ",commodityNames[stuff_it->first],stuff_it->second, constructionGroup->commodityRuleCount[stuff_it->first].maxload);
                ++*i;
            }//endif
        }//endfor
        for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++)
        {
            if(constructionGroup->commodityRuleCount[stuff_it->first].give
            && constructionGroup->commodityRuleCount[stuff_it->first].take
            && *i<14)
            {
                mps_store_ssddp(*i,"<->",commodityNames[stuff_it->first],stuff_it->second, constructionGroup->commodityRuleCount[stuff_it->first].maxload);
                ++*i;
            }//endif
        }//endfor
        for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++)
        {
            if((!constructionGroup->commodityRuleCount[stuff_it->first].give
            && !constructionGroup->commodityRuleCount[stuff_it->first].take)
            && *i<14)
            {
                mps_store_ssddp(*i,"--- ",commodityNames[stuff_it->first],stuff_it->second, constructionGroup->commodityRuleCount[stuff_it->first].maxload);
                ++*i;
            }//endif
        }//endfor
    }
    else // FLAG_EVACUATE
    {
        for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++)
        {
            if(*i<14)
            {
                mps_store_ssddp(*i,"<< ",commodityNames[stuff_it->first],stuff_it->second, constructionGroup->commodityRuleCount[stuff_it->first].maxload);
                ++*i;
            }//endif
        }//endfor
    }
}

void Construction::initialize_commodities(void)
{
    std::map<Commodities,CommodityRule>::iterator stuff_it;
    for(stuff_it = constructionGroup->commodityRuleCount.begin() ; stuff_it != constructionGroup->commodityRuleCount.end() ; stuff_it++)
    {
        commodityCount[stuff_it->first] = 0;
        setMemberSaved(&commodityCount[stuff_it->first], commodityNames[stuff_it->first]);
    }
}

void Construction::bootstrap_commodities(int percent)
{
    std::map<Commodities,CommodityRule>::iterator stuff_it;
    for(stuff_it = constructionGroup->commodityRuleCount.begin() ; stuff_it != constructionGroup->commodityRuleCount.end() ; stuff_it++)
    {
        if (stuff_it->first != STUFF_WASTE)
        {   commodityCount[stuff_it->first] = percent * stuff_it->second.maxload /100;}
    }
}

void Construction::report_commodities(void)
{
    std::map<Commodities, int>::iterator stuff_it;
    for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++)
    {
        tstat_census[stuff_it->first] += stuff_it->second;
        tstat_capacities[stuff_it->first] += constructionGroup->commodityRuleCount[stuff_it->first].maxload;
    }

}

void Construction::setCommodityRulesSaved(std::map<Commodities,CommodityRule> * stuffRuleCount)
{
    std::map<Commodities,CommodityRule>::iterator stuff_it;
    std::string giveStr = "give_";
    std::string takeStr = "take_";
    for( stuff_it = stuffRuleCount->begin() ; stuff_it != stuffRuleCount->end() ; stuff_it++)
    {
        setMemberSaved(&(stuff_it->second.give), giveStr + commodityNames[stuff_it->first]);
        setMemberSaved(&(stuff_it->second.take), takeStr + commodityNames[stuff_it->first]);
    }
}

int Construction::loadMember(std::string const &xml_tag, std::string const &xml_val)
{
    std::istringstream iss;
    iss.str(xml_val);
    if(memberRuleCount.count(xml_tag))
    {
        switch (memberRuleCount[xml_tag].memberType)
        {
            case TYPE_BOOL:
                iss>>*static_cast<bool *>(memberRuleCount[xml_tag].ptr);
                break;
            case TYPE_INT:
                iss>>*static_cast<int *>(memberRuleCount[xml_tag].ptr);
                break;
            case TYPE_USHORT:
                iss>>*static_cast<unsigned short *>(memberRuleCount[xml_tag].ptr);
                break;
             case TYPE_DOUBLE:
                iss>>*static_cast<double *>(memberRuleCount[xml_tag].ptr);
                break;
            case TYPE_FLOAT:
                iss>>*static_cast<float *>(memberRuleCount[xml_tag].ptr);
                break;
        }
        return 1;
    }
    else
    {
        //return silently in case of non optional xml tags
        return 0;
    }
}

int Construction::readbinaryMember(std::string const &xml_tag, gzFile fp)
{
    size_t s_t = 0;
//    if(memberRuleCount.count(xml_tag))
//    {
        switch (memberRuleCount[xml_tag].memberType)
        {
            case TYPE_BOOL:
                s_t = sizeof(bool);
                break;
            case TYPE_INT:
                s_t = sizeof(int);
                break;
            case TYPE_USHORT:
                 s_t = sizeof(short);
                break;
             case TYPE_DOUBLE:
                 s_t = sizeof(double);
                break;
            case TYPE_FLOAT:
                 s_t = sizeof(float);
                break;
        }
        gzread(fp,(char*)(memberRuleCount[xml_tag].ptr),s_t);
        return s_t;
/*    }
    else
    {
        assert(false);
        return 0;
    }
*/
}

void Construction::writeTemplate()
{

    std::string name;
    XMLTemplate * xml_tmp;
    unsigned short head = constructionGroup->group;
    if ((flags&FLAG_IS_TRANSPORT) && !binary_mode)
    {   name = "Transport";}
    else
    {   name = constructionGroup->name;}

    if ((!binary_mode && xml_template_libary.count(name) == 0)||
        (binary_mode && bin_template_libary.count(head) == 0))
    {
        xml_tmp = new XMLTemplate(name);
        if (!binary_mode)
        {
            xml_tmp->putTag("Group");
            xml_tmp->putTag("type");
            xml_tmp->putTag("map_x");
            xml_tmp->putTag("map_y");
        }
        std::map<std::string, MemberRule>::iterator member_it;
        for(member_it = memberRuleCount.begin() ; member_it != memberRuleCount.end() ; member_it++)
        {
            xml_tmp->putTag(member_it->first);
            size_t s_t = 4;
            switch (member_it->second.memberType)
            {
                case TYPE_BOOL:
                    s_t = sizeof(bool);
                    break;
                case TYPE_INT:
                    s_t = sizeof(int);
                    break;
                case TYPE_USHORT:
                    s_t = sizeof(unsigned short);
                    break;
                case TYPE_DOUBLE:
                    s_t = sizeof(double);
                    break;
                case TYPE_FLOAT:
                    s_t = sizeof(float);
                    break;
            }
            xml_tmp->add_len(s_t);
        }
        xml_tmp->rewind();
        if (binary_mode)
        {   xml_tmp->set_group(head);}
    }
}

void Construction::saveMembers(std::ostream *os)
{
    //make sure old types are actually valid
    if (constructionGroup->images_loaded && constructionGroup->graphicsInfoVector.size())
    {   type = type % constructionGroup->graphicsInfoVector.size();}
    std::string name;
    unsigned short head = constructionGroup->group;
    if (flags&FLAG_IS_TRANSPORT && !binary_mode)
    {   name = "Transport";}
    else
    {   name = constructionGroup->name;}
    if ((!binary_mode && xml_template_libary.count(name) == 0)||
        (binary_mode && bin_template_libary.count(head) == 0))
    {   writeTemplate();}
    XMLTemplate * xml_tmp;

    if (binary_mode)
    {   xml_tmp = bin_template_libary[head];}
    else
    {
        xml_tmp = xml_template_libary[name];
        if (os == &std::cout)
        {   xml_tmp->report(os);}
    }
    xml_tmp->rewind();
    size_t checksum = 0;
    std::map<std::string, MemberRule>::iterator member_it;
    if (binary_mode)
    {   //Mandatory header for binary files (before actual template)
        int idx = x + y * world.len();
        os->write( (char*) &head,sizeof(head));
        os->write( (char*) &constructionGroup->group,sizeof(constructionGroup->group));
        os->write( (char*) &type,sizeof(type));
        os->write( (char*) &idx,sizeof(idx));
    }
    else
    {   // Header for txt mode (part of template)
        *os <<"<" << name << ">" << constructionGroup->group << "\t";
        xml_tmp->step();
        *os << type << "\t";
        xml_tmp->step();
        *os << x << "\t";
        xml_tmp->step();
        *os << y << "\t";
        xml_tmp->step();
        os->flush();
    }
    while (!xml_tmp->reached_end())
    {
        member_it = memberRuleCount.find(xml_tmp->getTag());
        if (member_it != memberRuleCount.end())
        {

            if (!binary_mode)
            {
                switch (member_it->second.memberType)
                {
                    case TYPE_BOOL:
                        *os << *static_cast<bool *>(member_it->second.ptr);
                        break;
                    case TYPE_INT:
                        *os << *static_cast<int *>(member_it->second.ptr);
                        break;
                    case TYPE_USHORT:
                        *os << *static_cast<unsigned short *>(member_it->second.ptr);
                        break;
                    case TYPE_DOUBLE:
                        *os << *static_cast<double *>(member_it->second.ptr);
                        break;
                    case TYPE_FLOAT:
                        *os << *static_cast<float *>(member_it->second.ptr);
                        break;
                }
                *os << '\t';
            }
            else //binary mode
            {
                size_t s_t = 4;
                switch (member_it->second.memberType)
                {
                    case TYPE_BOOL:
                        s_t = sizeof(bool);
                        break;
                    case TYPE_INT:
                        s_t = sizeof(int);
                        break;
                    case TYPE_USHORT:
                        s_t = sizeof(unsigned short);
                        break;
                    case TYPE_DOUBLE:
                        s_t = sizeof(double);
                        break;
                    case TYPE_FLOAT:
                        s_t = sizeof(float);
                        break;
                }
                os->write( (char*) member_it->second.ptr,s_t);
                checksum += s_t;
            }
        }
        else
        {   std::cout << "ignored " << xml_tmp->getTag() << " in " << name << " template." <<std::endl;}
        xml_tmp->step();
    }
    if (!binary_mode)
    {   *os << "</" << name << ">" << std::endl;}
    else
    {
        if (xml_tmp->len() != checksum)
        {
            std::cout << xml_tmp->len() << " != " << checksum << std::endl;
            assert(checksum == xml_tmp->len());
        }
    }
}

//use this before deleting a construction. Construction requests check independently against NULL
void Construction::detach()
{
    //std::cout << "detaching: " << constructionGroup->name << std::endl;
    ::constructionCount.remove_construction(this);
    if(world(x,y)->construction == this)
    {   world(x,y)->construction = NULL;}
    for (unsigned short i = 0; i < constructionGroup->size; ++i)
    {
        for (unsigned short j = 0; j < constructionGroup->size; ++j)
        {
            // constructions may have children e.g. waste burning markets/shanties
            if(world(x+j,y+i)->construction)
            {
                //std::cout << "killing child: " << world(x+j,y+i)->construction->constructionGroup->name << std::endl;
                ::constructionCount.remove_construction(world(x+j,y+i)->construction);
                delete world(x+j,y+i)->construction;
                world(x+j,y+i)->construction = NULL;
            }
            world(x+j,y+i)->reportingConstruction = NULL;
        }
    }
    deneighborize();
}

void Construction::deneighborize()
{
    for(size_t i = 0; i < neighbors.size(); ++i)
    {
        std::vector<Construction*> *neib = &(neighbors[i]->neighbors);
        std::vector<Construction*>::iterator neib_it = neib->begin();
        while(neib_it != neib->end() && *neib_it != this)
            {++neib_it;}
/*#ifdef DEBUG
        assert(neib_it != neib->end());
#endif*/
        neib->erase(neib_it);
    }
    neighbors.clear();
    for(size_t i = 0; i < partners.size(); ++i)
    {
        std::vector<Construction*> *partner = &(partners[i]->partners);
        std::vector<Construction*>::iterator partner_it = partner->begin();
        while(partner_it != partner->end() && *partner_it != this)
            {++partner_it;}
/*#ifdef DEBUG
        assert(partner_it != partner->end());
#endif*/
        partner->erase(partner_it);
    }
    partners.clear();
    if (flags & FLAG_POWER_LINE)
    {
        world(x + 1, y)->flags &= ~FLAG_POWER_CABLES_90;
        world(x - 1, y)->flags &= ~FLAG_POWER_CABLES_90;
        world(x, y + 1)->flags &= ~FLAG_POWER_CABLES_0;
        world(x, y - 1)->flags &= ~FLAG_POWER_CABLES_0;
    }
}


void Construction::neighborize()
{
//skip ghosts (aka burning waste) and powerlines here
    if(!(flags & (FLAG_IS_GHOST | FLAG_POWER_LINE)) && (constructionGroup->group != GROUP_FIRE))
    {
        if(flags & FLAG_IS_TRANSPORT)//search adjacent tiles only
        {
            Construction* cst = NULL;
            Construction* cst1 = NULL;
            Construction* cst2 = NULL;
            Construction* cst3 = NULL;
            Construction* cst4 = NULL;
            unsigned short size = constructionGroup->size;
            for (unsigned short edge = 0; edge < size; ++edge)
            {
                //here we rely on invisible edge tiles
                cst = world(x - 1,y + edge)->reportingConstruction;
                if(cst && cst != cst1 && !(cst->flags & (FLAG_IS_GHOST | FLAG_POWER_LINE)) && (cst->constructionGroup->group != GROUP_FIRE))
                {   link_to(cst1 = cst);}
                cst = world(x + edge,y - 1)->reportingConstruction;
                if(cst && cst != cst2 && !(cst->flags & (FLAG_IS_GHOST | FLAG_POWER_LINE)) && (cst->constructionGroup->group != GROUP_FIRE))
                {   link_to(cst2 = cst);}
                cst = world(x + size,y + edge)->reportingConstruction;
                if(cst && cst != cst3 && !(cst->flags & (FLAG_IS_GHOST | FLAG_POWER_LINE)) && (cst->constructionGroup->group != GROUP_FIRE))
                {   link_to(cst3 = cst);}
                cst = world(x + edge,y + size)->reportingConstruction;
                if(cst && cst != cst4 && !(cst->flags & (FLAG_IS_GHOST | FLAG_POWER_LINE)) && (cst->constructionGroup->group != GROUP_FIRE))
                {   link_to(cst4 = cst);}
            }
        }
        else // search full market range for constructions
        {
            int tmp;
            int lenm1 = world.len()-1;
            tmp = x - GROUP_MARKET_RANGE - GROUP_MARKET_SIZE + 1;
            int xs = (tmp < 1) ? 1 : tmp;
            tmp = y - GROUP_MARKET_RANGE - GROUP_MARKET_SIZE + 1;
            int ys = (tmp < 1)? 1 : tmp;
            tmp = x + GROUP_MARKET_RANGE + 1;
            int xe = (tmp > lenm1) ? lenm1 : tmp;
            tmp = y + GROUP_MARKET_RANGE + 1;
            int ye = (tmp > lenm1)? lenm1 : tmp;

            for(int yy = ys; yy < ye; ++yy)
            {
                for(int xx = xs; xx < xe; ++xx)
                {
                    //dont search at home
                    if(((xx == x )  && (yy == y)))
                    {   continue;}
                    if(world(xx,yy)->construction) //be unique
                    {
                        Construction *cst = world(xx,yy)->reportingConstruction; //stick with reporting
                        if((cst->flags & FLAG_POWER_LINE) || (cst->constructionGroup->group == GROUP_FIRE))
                        {   continue;}
                        //will attempt to make a link
                        link_to(cst);
                    }
                }

            }
        }
    }
}

void Construction::link_to(Construction* other)
{
/*
    std::cout << "new link requested : " << constructionGroup->name << "(" << x << "," << y << ") - "
    << other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
*/
//Theses tests are only for rules of the game, simulation would run happily with duplicate/oneway links
/*#ifdef DEBUG
    std::vector<Construction*>::iterator neib_it = neighbors.begin();
    bool ignore = false;
    while(neib_it !=  neighbors.end() && *neib_it != other)
            {++neib_it;}

    if(neib_it != neighbors.end())
    {
        std::cout << "duplicate neighbor : " << constructionGroup->name << "(" << x << "," << y << ") - "
        << other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
        ignore = true;
    }
    neib_it = partners.begin();
    while(neib_it !=  partners.end() && *neib_it != other)
            {++neib_it;}

    if(neib_it != partners.end())
    {
        std::cout << "duplicate partner : " << constructionGroup->name << "(" << x << "," << y << ") - "
        << other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
        ignore = true;
    }
    if (this == other)
    {
        std::cout << "home link error : " << constructionGroup->name << "(" << x << "," << y << ") - "
        << other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
        ignore = true;
    }
    if(ignore)
    {   return;}
#endif*/
    bool useful = false;
    Commodities stuff_ID;
    std::map<Commodities, int>::iterator stuff_it;
    for(stuff_it = commodityCount.begin() ;!useful && stuff_it != commodityCount.end() ; stuff_it++ )
    {
        stuff_ID = stuff_it->first;
        if(other->commodityCount.count(stuff_ID))
        {
            useful=((constructionGroup->commodityRuleCount[stuff_ID].take &&
                other->constructionGroup->commodityRuleCount[stuff_ID].give)
            ||
               (constructionGroup->commodityRuleCount[stuff_ID].give &&
               other->constructionGroup->commodityRuleCount[stuff_ID].take));
        }
    }
    if (useful)
    {
        if(flags & FLAG_POWER_LINE)
        {
            neighbors.push_back(other);
            other->neighbors.push_back(this);
            //std::cout << "power link : " << constructionGroup->name << "(" << x << "," << y << ") - "
            //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
            return;
        }
        int vec_x = other->x - x;
        int vec_y = other->y - y;
        int ns = other->constructionGroup->size;
        int s = constructionGroup->size;
        //Check if *this is adjacent to *other
        if(!(((vec_x == s) || (vec_x == -ns)) && ((vec_y == s) || (vec_y == -ns))) &&
            (((vec_x <= s) && (vec_x >= -ns)) && ((vec_y <= s) && (vec_y >= -ns))))
        {
            neighbors.push_back(other);
            other->neighbors.push_back(this);
            //std::cout << "neighbor : " << constructionGroup->name << "(" << x << "," << y << ") - "
            //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
        }
        //transport may never be a distant partner
        else if(! ((flags | other->flags) & FLAG_IS_TRANSPORT))
        {
            if ((other->constructionGroup->group == GROUP_MARKET)^(constructionGroup->group == GROUP_MARKET))
            {
                partners.push_back(other);
                other->partners.push_back(this);
                //std::cout << "partner : " << constructionGroup->name << "(" << x << "," << y << ") - "
                //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
            }
            //else
            //std::cout << "rejected connection : " << constructionGroup->name << "(" << x << "," << y << ") - "
            //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
        }
        //else
        //std::cout << "to far transport : " << constructionGroup->name << "(" << x << "," << y << ") - "
        //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
    }
    //else
    //std::cout << "useless connection : " << constructionGroup->name << "(" << x << "," << y << ") - "
    //<< other->constructionGroup->name << "(" << other->x << "," << other->y << ")" << std::endl;
}

int Construction::tellstuff(Commodities stuff_ID, int center_ratio) //called by Minimap and connecttransport
{
    if (commodityCount.count(stuff_ID))
    {
        int loc_lvl = commodityCount[stuff_ID];
        int loc_cap = constructionGroup->commodityRuleCount[stuff_ID].maxload;
        if ((flags & FLAG_EVACUATE) && (center_ratio != -2))
        {   return (loc_lvl>1)?loc_lvl:-1;}

/*#ifdef DEBUG
        if (loc_lvl > loc_cap)
        {
            std::cout<<"fixed "<<commodityNames[stuff_ID]<<" > maxload at "<<constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
            commodityCount[stuff_ID] = loc_cap;
            loc_lvl = loc_cap;
        }
        if (loc_lvl < 0)
        {
            std::cout<<"fixed "<<commodityNames[stuff_ID]<<" < 0 at "<<constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
            commodityCount[stuff_ID] = loc_cap;
            loc_lvl = 0;
        }

        if (loc_cap < 1)
        {
            std::cout<<"maxload "<<commodityNames[stuff_ID]<<" <= 0 error at "<<constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;
        }
#endif*/
        int loc_ratio = loc_lvl * TRANSPORT_QUANTA / (loc_cap);
        //Tell actual stock if we would tentatively participate in transport
        if ((center_ratio < 0) || (
        loc_ratio>center_ratio?constructionGroup->commodityRuleCount[stuff_ID].give:
            constructionGroup->commodityRuleCount[stuff_ID].take) )
        {   return (loc_ratio);}
    }
    return -1;
}

void Construction::trade()
{
    int ratio, cap, lvl, center_lvl, center_cap;
    int traffic, max_traffic;
    Commodities stuff_ID;
    const size_t neighsize = neighbors.size();
    bool lvls[neighsize];
    std::map<Commodities, int>::iterator stuff_it;
    Transport *transport = NULL;
    Powerline *powerline = NULL;
    if(flags & FLAG_IS_TRANSPORT)
    {   transport = dynamic_cast<Transport*>(this);}
    else if(flags & FLAG_POWER_LINE)
    {   powerline = dynamic_cast<Powerline*>(this);}
    /*begin for over all different stuff*/
    for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++ )
    {
        stuff_ID = stuff_it->first;
        center_lvl = stuff_it->second;
        center_cap = constructionGroup->commodityRuleCount[stuff_ID].maxload;
        if(flags & FLAG_EVACUATE)
        {
            if(center_lvl > 0)
            {   center_lvl = center_cap;}
            else
            {   continue;} // next commodity
        }
        //first order approximation for ratio
        ratio = (center_lvl * TRANSPORT_QUANTA / (center_cap) );
        lvl = center_lvl;
        cap = center_cap;
        for(unsigned int i = 0; i < neighsize; ++i)
        {
            Construction *pear = neighbors[i];
            lvls[i] = false;
            if(pear->commodityCount.count(stuff_ID))
            {
                int lvlsi = pear->commodityCount[stuff_ID];
                int capsi = pear->constructionGroup->commodityRuleCount[stuff_ID].maxload;
                if(!(pear->flags & FLAG_EVACUATE))
                {
                    int pearat = lvlsi * TRANSPORT_QUANTA / capsi;
                    //only consider stuff that would tentatively move
                    if(((pearat > ratio)&&!(constructionGroup->commodityRuleCount[stuff_ID].take &&
                            pear->constructionGroup->commodityRuleCount[stuff_ID].give)) ||
                       ((pearat < ratio)&&!(constructionGroup->commodityRuleCount[stuff_ID].give &&
                            pear->constructionGroup->commodityRuleCount[stuff_ID].take)))
                    {   continue;}
                    lvl += lvlsi;
                    cap += capsi;
                }
                lvls[i] = true;
            }
        }
        ratio = lvl * TRANSPORT_QUANTA / cap;
        max_traffic = 0;
        int old_center = center_lvl;
        //make flow towards ratio
        for(unsigned int i = 0; i < neighsize; ++i)
        {
            if(lvls[i])
            {
                traffic = neighbors[i]->equilibrate_stuff(&center_lvl, center_cap, ratio, stuff_ID, constructionGroup);
                if( traffic > max_traffic )
                {   max_traffic = traffic;}
            }
        }
        int flow = center_lvl - old_center;//stuff_it->second;
        //do some smoothing to suppress fluctuations from random order
        // max possible ~90 %
        if(flags & FLAG_IS_TRANSPORT) //Special for transport
        {   transport->trafficCount[stuff_ID] = (9 * transport->trafficCount[stuff_ID] + max_traffic) / 10;}
        else if(flags & FLAG_POWER_LINE) //Special for powerlines
        {
            powerline->trafficCount[stuff_ID] = (9 * powerline->trafficCount[stuff_ID] + max_traffic) / 10;
            for(unsigned int i = 0; i < neighsize; ++i)
            {
                if((powerline->anim_counter == 0)
                && !(neighbors[i]->flags & FLAG_POWER_LINE)
                && neighbors[i]->constructionGroup->commodityRuleCount[stuff_ID].give
                && (neighbors[i]->commodityCount[stuff_ID] > 0))
                {   powerline->anim_counter = POWER_MODULUS + rand()%POWER_MODULUS;}
                if((powerline->flashing && (neighbors[i]->flags & FLAG_POWER_LINE)))
                {   ConstructionManager::submitRequest(new PowerLineFlashRequest(neighbors[i]));}
            }
        }
        else if ((flow > 0) && (constructionGroup->group != GROUP_MARKET))
        {
            switch (stuff_ID)
                {
                    case (STUFF_JOBS) :
                        income_tax += flow;
                        break;
                    case (STUFF_GOODS) :
                        goods_tax += flow;
                        goods_used += flow;
                    case (STUFF_COAL) :
                        coal_tax += flow;
                        break;
                    default:
                        break;
                }
        }
        stuff_it->second += flow; //update center_lvl
    } //endfor all different STUFF
}

int Construction::equilibrate_stuff(int *rem_lvl, int rem_cap , int ratio, Commodities stuff_ID, ConstructionGroup * rem_cstGroup)
{
    if (commodityCount.count(stuff_ID) ) // we know stuff_id
    {
        int flow, traffic;
        int *loc_lvl;
        int loc_cap;
        loc_lvl = &(commodityCount[stuff_ID]);
        loc_cap = constructionGroup->commodityRuleCount[stuff_ID].maxload;
        if (!(flags & FLAG_EVACUATE))
        {
            flow = (ratio * (loc_cap) / TRANSPORT_QUANTA) - (*loc_lvl);
            if (((flow > 0) && (!(constructionGroup->commodityRuleCount[stuff_ID].take &&
            rem_cstGroup->commodityRuleCount[stuff_ID].give) ))
            || ((flow < 0) && !(constructionGroup->commodityRuleCount[stuff_ID].give &&
            rem_cstGroup->commodityRuleCount[stuff_ID].take) ))
            {   //construction refuses the flow
                return 0;
            }
            if (flow > 0)
            {
                if (flow * TRANSPORT_RATE > rem_cap )
                {   flow = rem_cap / TRANSPORT_RATE;}
                if (flow > *rem_lvl)
                {   flow = *rem_lvl;}
            }
            else if (flow < 0)
            {
                if(-flow * TRANSPORT_RATE > rem_cap)
                {   flow = - rem_cap / TRANSPORT_RATE;}
                if(-flow > (rem_cap-*rem_lvl))
                {   flow = -(rem_cap-*rem_lvl);}
            }
            //std::cout.flush();
            if (!(flags & FLAG_IS_TRANSPORT) && (flow > 0)
                && (constructionGroup->group != GROUP_MARKET))
            //something is given to a consumer
            {
                switch (stuff_ID)
                {
                    case (STUFF_JOBS) :
                        income_tax += flow;
                        break;
                    case (STUFF_GOODS) :
                        goods_tax += flow;
                        goods_used += flow;
                    case (STUFF_COAL) :
                        coal_tax += flow;
                        break;
                    default:
                        break;
                }
            }
        }
        else // we are evacuating
        {
            flow = -(rem_cap-*rem_lvl);
            if (-flow > *loc_lvl)
            {   flow = -*loc_lvl;}
        }
        traffic = flow * TRANSPORT_QUANTA / rem_cap;
        // incomming and outgoing traffic dont cancel but add up
        if (traffic < 0)
        {
            traffic = -traffic;
        }
        *loc_lvl += flow;
        *rem_lvl -= flow;
        return traffic;
    }
    return -1; //there was nothing to handle
}

void Construction::playSound()
{
    int s = constructionGroup->chunks.size();
    if(constructionGroup->sounds_loaded && s)
    {   getSound()->playASound( constructionGroup->chunks[ rand()%s ] );}
}



//ConstructionGroup Declarations

void ConstructionGroup::growGraphicsInfoVector(void)
{
    graphicsInfoVector.resize(graphicsInfoVector.size() + 1);
}

int ConstructionGroup::getCosts() {
    return static_cast<int>
        (cost * (1.0f + (cost_mul * tech_level) / static_cast<float>(MAX_TECH_LEVEL)));
}

int ConstructionGroup::placeItem(int x, int y)
{
    Construction *tmpConstr = createConstruction(x, y);
#ifdef DEBUG
    if (tmpConstr == NULL)
    {
        std::cout << "failed to create " << name << " at " << "(" << x << ", " << y << ")" << std::endl;
        return -1;
    }
#endif
    //std::cout << "building: " << tmpConstr->constructionGroup->name  << "(" << x << "," << y << ")" << std::endl;
    //enforce empty site
    //unsigned short size = tmpConstr->constructionGroup->size;
    for (unsigned short i = 0; i < size; i++)
    {
        for (unsigned short j = 0; j < size; j++)
        {
            if (world(x+j, y+i)->reportingConstruction)
            {
                ConstructionManager::executeRequest
                ( new ConstructionDeletionRequest(world(x+j, y+i)->reportingConstruction));
            }
        }
    }

    world(x, y)->construction = tmpConstr;
    constructionCount.add_construction(tmpConstr); //register for Simulation
    for (unsigned short i = 0; i < size; i++)
    {
        for (unsigned short j = 0; j < size; j++)
        {
            world(x + j, y + i)->reportingConstruction = tmpConstr;
            if (!world(x + j, y + i)->is_water())
            {
                if(this != &powerlineConstructionGroup)
                {   world(x + j, y + i)->setTerrain(GROUP_DESERT);}
                else if (world(x + j, y + i)->group != GROUP_DESERT)
                {   world(x + j, y + i)->setTerrain(GROUP_BARE);}
            }
        } //endfor j
    }// endfor i
    //now look for neighbors
    //skip ghosts (aka burning waste) and powerlines here
    if(!(tmpConstr->flags & (FLAG_IS_GHOST | FLAG_POWER_LINE)) && (tmpConstr->constructionGroup->group != GROUP_FIRE))
    {   tmpConstr->neighborize();}
    return 0;
}

extern void ok_dial_box(const char *, int, const char *);

bool ConstructionGroup::is_allowed_here(int x, int y, bool msg)
{

    //handle transport quickly
    if(world.is_visible(x, y) && (group == GROUP_TRACK || group == GROUP_ROAD || group == GROUP_RAIL))
    {   return (world(x,y)->is_bare() ||
            world(x,y)->is_powerline() ||
    (world(x,y)->is_water() && !world(x,y)->is_transport()) ||
    ((world(x,y)->is_transport() && world(x,y)->getTransportGroup() != group)));
    }

    //now check for special rules
    switch (group)
    {
    case GROUP_SOLAR_POWER:
        if (total_money <= 0) {
            if (msg)
                ok_dial_box("no-credit-solar-power.mes", BAD, 0L);
            return false;
        }
        break;

    case GROUP_UNIVERSITY:
        if (total_money <= 0)
        {
            if (msg)
                ok_dial_box("no-credit-university.mes", BAD, 0L);
            return false;
        }
        else if ((Counted<School>::getInstanceCount()/4 - Counted<University>::getInstanceCount()) < 1)
        {
            if (msg)
                ok_dial_box("warning.mes", BAD, "Not enough students, build more schools.");
            return false;
        }
        break;

    case GROUP_RECYCLE:
        if (total_money <= 0) {
            if (msg)
                ok_dial_box("no-credit-recycle.mes", BAD, 0L);
            return false;
        }
        break;

    case GROUP_ROCKET:
        if (total_money <= 0) {
            if (msg)
                ok_dial_box("no-credit-rocket.mes", BAD, 0L);
            return false;
        }
        break;

    //The Harbour needs a River on the East side.
    //and relies on invisible tiles if attempted at edge
    case GROUP_PORT:
        for(int j = 0; j < size; j++ )
        {
            if (!( world(x + size, y + j)->flags & FLAG_IS_RIVER ) )
            {
                if (msg)
                    ok_dial_box("warning.mes", BAD, _("Port must be connected to river all along right side."));
                return false;
            }
        }
        break;

    //Waterwell needs ... water :-)
    case GROUP_WATERWELL:
        {
            bool has_ugw = false;
            for (int i = 0; i < size; i++)
                for (int j = 0; j < size; j++)
                    has_ugw = has_ugw | (world(x + j,y + i)->flags & FLAG_HAS_UNDERGROUND_WATER);
            if (!has_ugw)
            {
                if (msg)
                    ok_dial_box("warning.mes", BAD, _("You can't build a water well here: it is all desert."));
                return false;
            }
        }
        break;
    //Oremine
    /* GCS: mines over old mines is OK if there is enough remaining
     *  ore, as is the case when there is partial overlap. */
    case GROUP_OREMINE:
        {
            int total_ore = 0;
            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    total_ore += world(x+j, y+i)->ore_reserve;
                }
            }
            if (total_ore < MIN_ORE_RESERVE_FOR_MINE) {
                if (msg) ok_dial_box("warning.mes", BAD, _("You can't build a mine here: there is no ore left at this site"));
                return false; // not enought ore
            }
        }
    break;

    //Parkland
    case GROUP_PARKLAND:
        if (!(world(x, y)->flags & FLAG_HAS_UNDERGROUND_WATER))
        {
            if (msg)
                ok_dial_box("warning.mes", BAD, _("You can't build a park here: it is a desert, parks need water"));
            return false;
        }
        if (total_money <= 0) {
            if (msg)
                ok_dial_box("no-credit-parkland.mes", BAD, 0L);
            return false;
        }
        break;
    //Other cases
    }
    //double check cash
    if (no_credit && (total_money < 1))
    {
        if (msg)
        {
            ok_dial_box("warning.mes", BAD, "You cannot build this item on credit!");
        }
        return false;
    }
    //At last check for bare building site
    for(int j = 0; j<size; j++)
    {
        for(int i = 0; i<size; i++)
        {
            if(!world(x+i, y+j)->is_bare())
            {   return false;}
        }
    }
    return true;
}



void ConstructionGroup::printGroups()
{
    std::map<unsigned short, ConstructionGroup *>::iterator iterator;
    for (iterator = groupMap.begin(); iterator != groupMap.end(); iterator++)
    {
        std::cout << "group #" << iterator->first << ": " << iterator->second->name << std::endl;
    }
}

std::map<unsigned short, ConstructionGroup *> ConstructionGroup::groupMap;
std::map<std::string, ConstructionGroup *> ConstructionGroup::resourceMap;

//Legacy Stuff


struct TYPE main_types[NUM_OF_TYPES];

/* AL1 : 2.x : RANGE of buildings is not included in this struct GROUP
 *
 *      this is a bit annoying => would need to be fixed when(if?)
 *      all the struct and associated macros are cleaned (map, MP_INFO etc.)
 *
 *      for now (2.1 svn rev 1585) just put it in range.h
 *
 */

struct GROUP main_groups[NUM_OF_GROUPS] = {
    /* 0 */
    {N_("Empty"),               /* name */
     FALSE,                     /* need credit? */
     GROUP_BARE,                /* group number */
     1,                         /* size */
     (green(12)),               /* color */
     1,                         /* cost multiplier */
     1,                         /* bulldoze cost */
     0,                         /* probability of fire */
     0,                         /* cost */
     0                          /* tech */
     },

    /* 1 */
    {N_("Power line"),
     FALSE,                     /* need credit? */
     GROUP_POWER_LINE,
     GROUP_POWER_LINE_SIZE,
     GROUP_POWER_LINE_COLOUR,
     GROUP_POWER_LINE_COST_MUL,
     GROUP_POWER_LINE_BUL_COST,
     GROUP_POWER_LINE_FIREC,
     GROUP_POWER_LINE_COST,
     GROUP_POWER_LINE_TECH},

    /* 2 */
    {N_("Solar PS"),
     TRUE,                      /* need credit? */
     GROUP_SOLAR_POWER,
     GROUP_SOLAR_POWER_SIZE,
     GROUP_SOLAR_POWER_COLOUR,
     GROUP_SOLAR_POWER_COST_MUL,
     GROUP_SOLAR_POWER_BUL_COST,
     GROUP_SOLAR_POWER_FIREC,
     GROUP_SOLAR_POWER_COST,
     GROUP_SOLAR_POWER_TECH},
    /* 3 */
    {N_("Substation"),
     FALSE,                     /* need credit? */
     GROUP_SUBSTATION,
     GROUP_SUBSTATION_SIZE,
     GROUP_SUBSTATION_COLOUR,
     GROUP_SUBSTATION_COST_MUL,
     GROUP_SUBSTATION_BUL_COST,
     GROUP_SUBSTATION_FIREC,
     GROUP_SUBSTATION_COST,
     GROUP_SUBSTATION_TECH},
    /* 4 */
    {N_("Residential LL"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_LL,
     GROUP_RESIDENCE_SIZE,
     GROUP_RESIDENCE_LL_COLOUR,
     GROUP_RESIDENCE_LL_COST_MUL,
     GROUP_RESIDENCE_LL_BUL_COST,
     GROUP_RESIDENCE_LL_FIREC,
     GROUP_RESIDENCE_LL_COST,
     GROUP_RESIDENCE_LL_TECH},
    /* 5 */
    {N_("Farm"),
     FALSE,                     /* need credit? */
     GROUP_ORGANIC_FARM,
     GROUP_ORGANIC_FARM_SIZE,
     GROUP_ORGANIC_FARM_COLOUR,
     GROUP_ORGANIC_FARM_COST_MUL,
     GROUP_ORGANIC_FARM_BUL_COST,
     GROUP_ORGANIC_FARM_FIREC,
     GROUP_ORGANIC_FARM_COST,
     GROUP_ORGANIC_FARM_TECH},
    /* 6 */
    {N_("Market"),
     FALSE,                     /* need credit? */
     GROUP_MARKET,
     GROUP_MARKET_SIZE,
     GROUP_MARKET_COLOUR,
     GROUP_MARKET_COST_MUL,
     GROUP_MARKET_BUL_COST,
     GROUP_MARKET_FIREC,
     GROUP_MARKET_COST,
     GROUP_MARKET_TECH},
    /* 7 */
    {N_("Track"),
     FALSE,                     /* need credit? */
     GROUP_TRACK,
     GROUP_TRANSPORT_SIZE,
     GROUP_TRACK_COLOUR,
     GROUP_TRACK_COST_MUL,
     GROUP_TRACK_BUL_COST,
     GROUP_TRACK_FIREC,
     GROUP_TRACK_COST,
     GROUP_TRACK_TECH},
    /* 8 */
    {N_("Coalmine"),
     FALSE,                     /* need credit? */
     GROUP_COALMINE,
     GROUP_TRANSPORT_SIZE,
     GROUP_COALMINE_COLOUR,
     GROUP_COALMINE_COST_MUL,
     GROUP_COALMINE_BUL_COST,
     GROUP_COALMINE_FIREC,
     GROUP_COALMINE_COST,
     GROUP_COALMINE_TECH},
    /* 9 */
    {N_("Railway"),
     FALSE,                     /* need credit? */
     GROUP_RAIL,
     GROUP_TRANSPORT_SIZE,
     GROUP_RAIL_COLOUR,
     GROUP_RAIL_COST_MUL,
     GROUP_RAIL_BUL_COST,
     GROUP_RAIL_FIREC,
     GROUP_RAIL_COST,
     GROUP_RAIL_TECH},
    /* 10 */
    {N_("Coal PS"),
     FALSE,                     /* need credit? */
     GROUP_COAL_POWER,
     GROUP_COAL_POWER_SIZE,
     GROUP_COAL_POWER_COLOUR,
     GROUP_COAL_POWER_COST_MUL,
     GROUP_COAL_POWER_BUL_COST,
     GROUP_COAL_POWER_FIREC,
     GROUP_COAL_POWER_COST,
     GROUP_COAL_POWER_TECH},
    /* 11 */
    {N_("Road"),
     FALSE,                     /* need credit? */
     GROUP_ROAD,
     GROUP_TRANSPORT_SIZE,
     GROUP_ROAD_COLOUR,
     GROUP_ROAD_COST_MUL,
     GROUP_ROAD_BUL_COST,
     GROUP_ROAD_FIREC,
     GROUP_ROAD_COST,
     GROUP_ROAD_TECH},
    /* 12 */
    {N_("Lt. Industry"),
     FALSE,                     /* need credit? */
     GROUP_INDUSTRY_L,
     GROUP_INDUSTRY_L_SIZE,
     GROUP_INDUSTRY_L_COLOUR,
     GROUP_INDUSTRY_L_COST_MUL,
     GROUP_INDUSTRY_L_BUL_COST,
     GROUP_INDUSTRY_L_FIREC,
     GROUP_INDUSTRY_L_COST,
     GROUP_INDUSTRY_L_TECH},
    /* 13 */
    {N_("University"),
     FALSE,                     /* need credit? */
     GROUP_UNIVERSITY,
     GROUP_UNIVERSITY_SIZE,
     GROUP_UNIVERSITY_COLOUR,
     GROUP_UNIVERSITY_COST_MUL,
     GROUP_UNIVERSITY_BUL_COST,
     GROUP_UNIVERSITY_FIREC,
     GROUP_UNIVERSITY_COST,
     GROUP_UNIVERSITY_TECH},
    /* 14 */
    {N_("Commune"),
     FALSE,                     /* need credit? */
     GROUP_COMMUNE,
     GROUP_COMMUNE_SIZE,
     GROUP_COMMUNE_COLOUR,
     GROUP_COMMUNE_COST_MUL,
     GROUP_COMMUNE_BUL_COST,
     GROUP_COMMUNE_FIREC,
     GROUP_COMMUNE_COST,
     GROUP_COMMUNE_TECH},
    /* 15 */
    {N_("Ore mine"),
     FALSE,                     /* need credit? */
     GROUP_OREMINE,
     GROUP_OREMINE_SIZE,
     GROUP_OREMINE_COLOUR,
     GROUP_OREMINE_COST_MUL,
     GROUP_OREMINE_BUL_COST,
     GROUP_OREMINE_FIREC,
     GROUP_OREMINE_COST,
     GROUP_OREMINE_TECH},
    /* 16 */
    {N_("Rubbish tip"),
     FALSE,                     /* need credit? */
     GROUP_TIP,
     GROUP_TIP_SIZE,
     GROUP_TIP_COLOUR,
     GROUP_TIP_COST_MUL,
     GROUP_TIP_BUL_COST,
     GROUP_TIP_FIREC,
     GROUP_TIP_COST,
     GROUP_TIP_TECH},
    /* 17 */
    {N_("Port"),
     FALSE,                     /* need credit? */
     GROUP_PORT,
     GROUP_PORT_SIZE,
     GROUP_PORT_COLOUR,
     GROUP_PORT_COST_MUL,
     GROUP_PORT_BUL_COST,
     GROUP_PORT_FIREC,
     GROUP_PORT_COST,
     GROUP_PORT_TECH},
    /* 18 */
    {N_("Hv. Industry"),
     FALSE,                     /* need credit? */
     GROUP_INDUSTRY_H,
     GROUP_INDUSTRY_H_SIZE,
     GROUP_INDUSTRY_H_COLOUR,
     GROUP_INDUSTRY_H_COST_MUL,
     GROUP_INDUSTRY_H_BUL_COST,
     GROUP_INDUSTRY_H_FIREC,
     GROUP_INDUSTRY_H_COST,
     GROUP_INDUSTRY_H_TECH},
    /* 19 */
    {N_("Park"),
     TRUE,                      /* need credit? */
     GROUP_PARKLAND,
     GROUP_PARKLAND_SIZE,
     GROUP_PARKLAND_COLOUR,
     GROUP_PARKLAND_COST_MUL,
     GROUP_PARKLAND_BUL_COST,
     GROUP_PARKLAND_FIREC,
     GROUP_PARKLAND_COST,
     GROUP_PARKLAND_TECH},
    /* 20 */
    {N_("Recycle"),
     TRUE,                      /* need credit? */
     GROUP_RECYCLE,
     GROUP_RECYCLE_SIZE,
     GROUP_RECYCLE_COLOUR,
     GROUP_RECYCLE_COST_MUL,
     GROUP_RECYCLE_BUL_COST,
     GROUP_RECYCLE_FIREC,
     GROUP_RECYCLE_COST,
     GROUP_RECYCLE_TECH},
    /* 21 */
    {N_("Water"),
     FALSE,                     /* need credit? */
     GROUP_WATER,
     1,
     GROUP_WATER_COLOUR,
     GROUP_WATER_COST_MUL,
     GROUP_WATER_BUL_COST,
     GROUP_WATER_FIREC,
     GROUP_WATER_COST,
     GROUP_WATER_TECH},
    /* 22 */
    {N_("Health centre"),
     FALSE,                     /* need credit? */
     GROUP_HEALTH,
     GROUP_HEALTH_SIZE,
     GROUP_HEALTH_COLOUR,
     GROUP_HEALTH_COST_MUL,
     GROUP_HEALTH_BUL_COST,
     GROUP_HEALTH_FIREC,
     GROUP_HEALTH_COST,
     GROUP_HEALTH_TECH},
    /* 23 */
    {N_("Rocket pad"),
     TRUE,                      /* need credit? */
     GROUP_ROCKET,
     GROUP_ROCKET_SIZE,
     GROUP_ROCKET_COLOUR,
     GROUP_ROCKET_COST_MUL,
     GROUP_ROCKET_BUL_COST,
     GROUP_ROCKET_FIREC,
     GROUP_ROCKET_COST,
     GROUP_ROCKET_TECH},
    /* 24 */
    {N_("Windmill"),
     FALSE,                     /* need credit? */
     GROUP_WINDMILL,
     GROUP_WINDMILL_SIZE,
     GROUP_WINDMILL_COLOUR,
     GROUP_WINDMILL_COST_MUL,
     GROUP_WINDMILL_BUL_COST,
     GROUP_WINDMILL_FIREC,
     GROUP_WINDMILL_COST,
     GROUP_WINDMILL_TECH},
    /* 25 */
    {N_("Monument"),
     FALSE,                     /* need credit? */
     GROUP_MONUMENT,
     GROUP_MONUMENT_SIZE,
     GROUP_MONUMENT_COLOUR,
     GROUP_MONUMENT_COST_MUL,
     GROUP_MONUMENT_BUL_COST,
     GROUP_MONUMENT_FIREC,
     GROUP_MONUMENT_COST,
     GROUP_MONUMENT_TECH},
    /* 26 */
    {N_("School"),
     FALSE,                     /* need credit? */
     GROUP_SCHOOL,
     GROUP_SCHOOL_SIZE,
     GROUP_SCHOOL_COLOUR,
     GROUP_SCHOOL_COST_MUL,
     GROUP_SCHOOL_BUL_COST,
     GROUP_SCHOOL_FIREC,
     GROUP_SCHOOL_COST,
     GROUP_SCHOOL_TECH},
    /* 27 */
    {N_("Blacksmith"),
     FALSE,                     /* need credit? */
     GROUP_BLACKSMITH,
     GROUP_BLACKSMITH_SIZE,
     GROUP_BLACKSMITH_COLOUR,
     GROUP_BLACKSMITH_COST_MUL,
     GROUP_BLACKSMITH_BUL_COST,
     GROUP_BLACKSMITH_FIREC,
     GROUP_BLACKSMITH_COST,
     GROUP_BLACKSMITH_TECH},
    /* 28 */
    {N_("Mill"),
     FALSE,                     /* need credit? */
     GROUP_MILL,
     GROUP_MILL_SIZE,
     GROUP_MILL_COLOUR,
     GROUP_MILL_COST_MUL,
     GROUP_MILL_BUL_COST,
     GROUP_MILL_FIREC,
     GROUP_MILL_COST,
     GROUP_MILL_TECH},
    /* 29 */
    {N_("Pottery"),
     FALSE,                     /* need credit? */
     GROUP_POTTERY,
     GROUP_POTTERY_SIZE,
     GROUP_POTTERY_COLOUR,
     GROUP_POTTERY_COST_MUL,
     GROUP_POTTERY_BUL_COST,
     GROUP_POTTERY_FIREC,
     GROUP_POTTERY_COST,
     GROUP_POTTERY_TECH},
    /* 30 */
    {N_("Fire sta'n"),
     FALSE,                     /* need credit? */
     GROUP_FIRESTATION,
     GROUP_FIRESTATION_SIZE,
     GROUP_FIRESTATION_COLOUR,
     GROUP_FIRESTATION_COST_MUL,
     GROUP_FIRESTATION_BUL_COST,
     GROUP_FIRESTATION_FIREC,
     GROUP_FIRESTATION_COST,
     GROUP_FIRESTATION_TECH},
    /* 31 */
    {N_("Sports field"),
     FALSE,                     /* need credit? */
     GROUP_CRICKET,
     GROUP_CRICKET_SIZE,
     GROUP_CRICKET_COLOUR,
     GROUP_CRICKET_COST_MUL,
     GROUP_CRICKET_BUL_COST,
     GROUP_CRICKET_FIREC,
     GROUP_CRICKET_COST,
     GROUP_CRICKET_TECH},
    /* 32 */
    /* there is no button for this. */
    {N_("Burnt"),
     FALSE,                     /* need credit? */
     GROUP_BURNT,
     1,                         /* size */
     GROUP_BURNT_COLOUR,
     GROUP_BURNT_COST_MUL,
     GROUP_BURNT_BUL_COST,
     GROUP_BURNT_FIREC,
     GROUP_BURNT_COST,
     GROUP_BURNT_TECH},
    /* 33 */
    /* there is no button for this. */
    {N_("Shanty town"),
     FALSE,                     /* need credit? */
     GROUP_SHANTY,
     GROUP_SHANTY_SIZE,
     GROUP_SHANTY_COLOUR,
     GROUP_SHANTY_COST_MUL,
     GROUP_SHANTY_BUL_COST,
     GROUP_SHANTY_FIREC,
     GROUP_SHANTY_COST,
     GROUP_SHANTY_TECH},
    /* 34 */
    /* there is no button for this. */
    {N_("Fire"),
     FALSE,                     /* need credit? */
     GROUP_FIRE,
     GROUP_FIRE_SIZE,
     GROUP_FIRE_COLOUR,
     GROUP_FIRE_COST_MUL,
     GROUP_FIRE_BUL_COST,
     GROUP_FIRE_FIREC,
     GROUP_FIRE_COST,
     GROUP_FIRE_TECH},
    /* 35 */
    /* there is no button for this. */
    {N_("Used"),
     FALSE,                     /* need credit? */
     GROUP_USED,
     1,                         /* size */
     GROUP_USED_COLOUR,
     GROUP_USED_COST_MUL,
     GROUP_USED_BUL_COST,
     GROUP_USED_FIREC,
     GROUP_USED_COST,
     GROUP_USED_TECH},
    /* 36 */
    {N_("Residential ML"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_ML,
     GROUP_RESIDENCE_SIZE,
     GROUP_RESIDENCE_ML_COLOUR,
     GROUP_RESIDENCE_ML_COST_MUL,
     GROUP_RESIDENCE_ML_BUL_COST,
     GROUP_RESIDENCE_ML_FIREC,
     GROUP_RESIDENCE_ML_COST,
     GROUP_RESIDENCE_ML_TECH},
    /* 37 */
    {N_("Residential HL"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_HL,
     GROUP_RESIDENCE_SIZE,
     GROUP_RESIDENCE_HL_COLOUR,
     GROUP_RESIDENCE_HL_COST_MUL,
     GROUP_RESIDENCE_HL_BUL_COST,
     GROUP_RESIDENCE_HL_FIREC,
     GROUP_RESIDENCE_HL_COST,
     GROUP_RESIDENCE_HL_TECH},
    /* 38 */
    {N_("Residential LH"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_LH,
     GROUP_RESIDENCE_SIZE,
     GROUP_RESIDENCE_LH_COLOUR,
     GROUP_RESIDENCE_LH_COST_MUL,
     GROUP_RESIDENCE_LH_BUL_COST,
     GROUP_RESIDENCE_LH_FIREC,
     GROUP_RESIDENCE_LH_COST,
     GROUP_RESIDENCE_LH_TECH},
    /* 39 */
    {N_("Residential MH"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_MH,
     GROUP_RESIDENCE_SIZE,
     GROUP_RESIDENCE_MH_COLOUR,
     GROUP_RESIDENCE_MH_COST_MUL,
     GROUP_RESIDENCE_MH_BUL_COST,
     GROUP_RESIDENCE_MH_FIREC,
     GROUP_RESIDENCE_MH_COST,
     GROUP_RESIDENCE_MH_TECH},
    /* 40 */
    {N_("Residential HH"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_HH,
     GROUP_RESIDENCE_SIZE,
     GROUP_RESIDENCE_HH_COLOUR,
     GROUP_RESIDENCE_HH_COST_MUL,
     GROUP_RESIDENCE_HH_BUL_COST,
     GROUP_RESIDENCE_HH_FIREC,
     GROUP_RESIDENCE_HH_COST,
     GROUP_RESIDENCE_HH_TECH},
    /* 41 */
    {N_("Water well"),
     FALSE,                     /* need credit? */
     GROUP_WATERWELL,
     GROUP_WATERWELL_SIZE,                         /* size */
     GROUP_WATERWELL_COLOUR,
     GROUP_WATERWELL_COST_MUL,
     GROUP_WATERWELL_BUL_COST,
     GROUP_WATERWELL_FIREC,
     GROUP_WATERWELL_COST,
     GROUP_WATERWELL_TECH},

    /* 42 */
    {N_("Desert"),              /* name */
     FALSE,                     /* need credit? */
     GROUP_DESERT,              /* group number */
     1,                         /* size */
     GROUP_DESERT_COLOUR,       /* color */
     GROUP_DESERT_COST_MUL,     /* cost multiplier */
     GROUP_DESERT_BUL_COST,     /* bulldoze cost */
     GROUP_DESERT_FIREC,        /* probability of fire */
     GROUP_DESERT_COST,         /* cost */
     GROUP_DESERT_TECH          /* tech */
     },
    /* 43 */
    {N_("Tree"),                /* name */
     FALSE,                     /* need credit? */
     GROUP_TREE,                /* group number */
     1,                         /* size */
     GROUP_TREE_COLOUR,         /* color */
     GROUP_TREE_COST_MUL,       /* cost multiplier */
     GROUP_TREE_BUL_COST,       /* bulldoze cost */
     GROUP_TREE_FIREC,          /* probability of fire */
     GROUP_TREE_COST,           /* cost */
     GROUP_TREE_TECH            /* tech */
     },

    /* 44 */
    {N_("Trees"),               /* name */
     FALSE,                     /* need credit? */
     GROUP_TREE2,               /* group number */
     1,                         /* size */
     GROUP_TREE2_COLOUR,        /* color */
     GROUP_TREE2_COST_MUL,      /* cost multiplier */
     GROUP_TREE2_BUL_COST,      /* bulldoze cost */
     GROUP_TREE2_FIREC,         /* probability of fire */
     GROUP_TREE2_COST,          /* cost */
     GROUP_TREE2_TECH           /* tech */
     },

    /* 45 */
    {N_("Forest"),              /* name */
     FALSE,                     /* need credit? */
     GROUP_TREE3,               /* group number */
     1,                         /* size */
     GROUP_TREE3_COLOUR,        /* color */
     GROUP_TREE3_COST_MUL,      /* cost multiplier */
     GROUP_TREE3_BUL_COST,      /* bulldoze cost */
     GROUP_TREE3_FIREC,         /* probability of fire */
     GROUP_TREE3_COST,          /* cost */
     GROUP_TREE3_TECH           /* tech */
     },

    /* 46 */
    {N_("Track Bridge"),
     FALSE,                     /* need credit? */
     GROUP_TRACK_BRIDGE,
     GROUP_TRANSPORT_SIZE,
     GROUP_TRACK_BRIDGE_COLOUR,
     GROUP_TRACK_BRIDGE_COST_MUL,
     GROUP_TRACK_BRIDGE_BUL_COST,
     GROUP_TRACK_BRIDGE_FIREC,
     GROUP_TRACK_BRIDGE_COST,
     GROUP_TRACK_BRIDGE_TECH},

     /* 47 */
    {N_("Road Bridge"),
     FALSE,                     /* need credit? */
     GROUP_ROAD_BRIDGE,
     GROUP_TRANSPORT_SIZE,
     GROUP_ROAD_BRIDGE_COLOUR,
     GROUP_ROAD_BRIDGE_COST_MUL,
     GROUP_ROAD_BRIDGE_BUL_COST,
     GROUP_ROAD_BRIDGE_FIREC,
     GROUP_ROAD_BRIDGE_COST,
     GROUP_ROAD_BRIDGE_TECH},

     /* 48 */
    {N_("Rail Bridge"),
     FALSE,                     /* need credit? */
     GROUP_RAIL_BRIDGE,
     GROUP_TRANSPORT_SIZE,
     GROUP_RAIL_BRIDGE_COLOUR,
     GROUP_RAIL_BRIDGE_COST_MUL,
     GROUP_RAIL_BRIDGE_BUL_COST,
     GROUP_RAIL_BRIDGE_FIREC,
     GROUP_RAIL_BRIDGE_COST,
     GROUP_RAIL_BRIDGE_TECH},

    /* 49 */
    {N_("Wind Power"),
     TRUE,                     /* need credit? */
     GROUP_WIND_POWER,
     GROUP_WIND_POWER_SIZE,
     GROUP_WIND_POWER_COLOUR,
     GROUP_WIND_POWER_COST_MUL,
     GROUP_WIND_POWER_BUL_COST,
     GROUP_WIND_POWER_FIREC,
     GROUP_WIND_POWER_COST,
     GROUP_WIND_POWER_TECH},

     /* 50 */
    /* End of Data */
    {"EOF",
     FALSE,                     /* need credit? */
     0,
     0,                         /* size */
     0,
     0,
     0,
     0,
     0,
     0}
};

unsigned short get_group_of_type(unsigned short type)
{
    if (type == CST_NONE)
    {   return GROUP_BARE;}
    return main_types[type].group;
}

void set_map_groups(void)
{
    const int len = world.len();
    const int area = len * len;
    for (int index = 0; index < area; ++index)
    {   world(index)->group = get_group_of_type(world(index)->type);}
}

/*

int get_group_cost(short group)
{
    return (int)((float)main_groups[group].cost
                 + ((float)main_groups[group].cost * ((float)main_groups[group].cost_mul * (float)tech_level)
                    / (float)MAX_TECH_LEVEL));
}

void get_type_name(short type, char *s)
{
    short g;
    g = get_group_of_type(type);
    strcpy(s, _(main_groups[g].name));
}

int get_type_cost(short type)
{
    return get_group_cost((short)get_group_of_type(type));
}
*/
/** @file lincity/lintypes.cpp */

