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
//#include "power.h"
#include <iostream>

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
//may be used for Terrain types only. returns silently for constructions

MapTile::MapTile():ground()
{
    //ground = Ground();
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
{
    if (construction)
    {
        ::constructionCount.remove_construction(construction);
        delete(construction);
        construction = NULL;
        // CK devalidates other mapTiles reportingConstructions
        // should not be a problem outside running Game
    }
}

void MapTile::setTerrain(unsigned short new_type)
{
    int new_group;
    if ((new_group = get_group_of_type(new_type)) < 0)
        return;
    if (ConstructionGroup::countConstructionGroup(new_group))
        return;
    this->type = new_type;
    this->group = new_group;
}


unsigned short MapTile::getType() //type of bare land or the covering construction
{
        return (reportingConstruction ? reportingConstruction->type : type);
}

unsigned short MapTile::getTopType() //type of bare land or the actual construction
{
        return (construction ? construction->type : type);
}

unsigned short MapTile::getGroup() //group of bare land or the covering construction
{
        return (reportingConstruction ? reportingConstruction->constructionGroup->group : group);
}

unsigned short MapTile::getTopGroup() //group of bare land or the actual construction
{
        return (construction ? construction->constructionGroup->group : group);
}

bool MapTile::is_bare() //true if we there is neither a covering construction nor water
{
    return (!reportingConstruction) && (get_group_of_type(type) != GROUP_WATER);
}

bool MapTile::is_water() //true on bridges or lakes (also under bridges)
{
    return (group == GROUP_WATER);
}

bool MapTile::is_lake() //true on lakes (also under bridges)
{
    return (group == GROUP_WATER) && !(flags & FLAG_IS_RIVER);
}

bool MapTile::is_river() // true on rivers (also under bridges)
{
    return (flags & FLAG_IS_RIVER);
}

bool MapTile::is_visible() // true if tile is not covered by another construction. Only useful for minimap Gameview is rotated to upperleft
{
    return (construction || !reportingConstruction);
}

bool MapTile::is_transport() //true on tracks, road, rails and bridges
{
    return (reportingConstruction && reportingConstruction->flags & FLAG_IS_TRANSPORT);
}

bool MapTile::is_residence() //true on residences
{
    return (reportingConstruction && (
        (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_LL)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_ML)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_HL)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_LH)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_MH)
     || (reportingConstruction->constructionGroup->group == GROUP_RESIDENCE_HH)
                                    )
           );
}

void MapTile::writeTemplate()
{
    std::string xml_tag;
    if ((!binary_mode && xml_template_libary.count("tile") == 0)
		|| (binary_mode && bin_template_libary.count(GROUP_DESERT) == 0) )
    {
        
        XMLTemplate * xml_tmp = new XMLTemplate("tile");
        //xml_tmp->add_len(sizeof(group));//for the head
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
        {	xml_tmp->set_group(GROUP_DESERT);}
    }
}


void MapTile::saveMembers(std::ostream *os)
{
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
    {	xml_tmp = bin_template_libary[head];}
    else 
    {
		xml_tmp = xml_template_libary["tile"];
		if (os == &std::cout)
		{	xml_tmp->report(os);}
    }
    xml_tmp->rewind();
    //TODO send template definition to console
    if (os == &std::cout);
    
    
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
			if (xml_tag == "flags")             	 {os->write( (char*)&flags,sizeof(flags));cm+=sizeof(flags);}
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

//FIXME How to incorporate this into construction?
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
    for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++)
    {
        if(constructionGroup->commodityRuleCount[stuff_it->first].take
        && ! constructionGroup->commodityRuleCount[stuff_it->first].give
        && *i < 14)
        {
            mps_store_ssddp(*i,"-->",commodityNames[stuff_it->first],stuff_it->second, constructionGroup->commodityRuleCount[stuff_it->first].maxload);
            ++*i;
        }//endif
    } //endfor
    for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++)
    {
        if(constructionGroup->commodityRuleCount[stuff_it->first].give
        && ! constructionGroup->commodityRuleCount[stuff_it->first].take
        && *i<14)
        {
            mps_store_ssddp(*i,"<--",commodityNames[stuff_it->first],stuff_it->second, constructionGroup->commodityRuleCount[stuff_it->first].maxload);
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
        if(!constructionGroup->commodityRuleCount[stuff_it->first].give
        && !constructionGroup->commodityRuleCount[stuff_it->first].take
        && *i<14)
        {
            mps_store_ssddp(*i,"---",commodityNames[stuff_it->first],stuff_it->second, constructionGroup->commodityRuleCount[stuff_it->first].maxload);
            ++*i;
        }//endif
    }//endfor
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


void Construction::list_connections(int * i)
{
    char p[] = {'_','_','_','_' ,'\0'};
    if (flags & FLAG_LEFT)
        p[0] = 'l';
    if (flags & FLAG_UP)
        p[1] = 'u';
    if (flags & FLAG_RIGHT)
        p[2] = 'r';
    if (flags & FLAG_DOWN)
        p[3] = 'd';
    if (*i<14)
    {
        mps_store_ss(*i,"Connected to", p);
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
    std::map<std::string, MemberRule>::iterator member_it;
    std::string name;
    XMLTemplate * xml_tmp;
    unsigned short head = constructionGroup->group;
    if (flags&FLAG_IS_TRANSPORT)
    {
        name = "Transport";
        head = GROUP_TRACK;
    }
    else
    {
        name = constructionGroup->name;
    }

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
						
		//xml_tmp->add_len(sizeof(constructionGroup->group) + sizeof(type) + sizeof(x));	//header + group entry
		
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
		{	
			xml_tmp->set_group(head);
		}
    }
}

void Construction::saveMembers(std::ostream *os)
{
    
    std::string name;
    unsigned short head = constructionGroup->group;
    if (flags&FLAG_IS_TRANSPORT)
    {
        name = "Transport";
        head = GROUP_TRACK;
    }
    else
    {
        name = constructionGroup->name;
    }

    if ((!binary_mode && xml_template_libary.count(name) == 0)||
		(binary_mode && bin_template_libary.count(head) == 0))
    {
        //std::cout << "creating xml template for " << name << std::endl;
        writeTemplate();
    }
    XMLTemplate * xml_tmp;
    
    if (binary_mode)
    {	xml_tmp = bin_template_libary[head];}
	else
	{	
		xml_tmp = xml_template_libary[name];
		if (os == &std::cout)
		{	xml_tmp->report(os);}	
	}
    xml_tmp->rewind(); 
    size_t checksum = 0;
    std::map<std::string, MemberRule>::iterator member_it;
    if (binary_mode)
	{   //Mandatory header for binary files (before actual template)
		int idx = x + y * world.len();
		os->write( (char*) &head,sizeof(head));
		os->write( (char*) &constructionGroup->group,sizeof(constructionGroup->group));
		//checksum += sizeof(constructionGroup->group); //head
		os->write( (char*) &type,sizeof(type));
		//checksum += sizeof(type); //head
		os->write( (char*) &idx,sizeof(idx));
		//checksum += sizeof(idx); //head
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
				//os->flush();
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
        {
            std::cout << "ignored " << xml_tmp->getTag() << " in " << name << " template." <<std::endl;
        }
        xml_tmp->step();

    }
    if (!binary_mode)
    {
		*os << "</" << name << ">" << std::endl;
    }
    else
    {	
		if (xml_tmp->len() != checksum)
		{
			std::cout << xml_tmp->len() << " != " << checksum << std::endl;
			assert(checksum == xml_tmp->len());
		}
    }
}

//ConstructionGroup Declarations

int ConstructionGroup::getCosts() {
    return static_cast<int>(
        cost * (1.0f + (cost_mul * tech_level) / static_cast<float>(MAX_TECH_LEVEL))
    );
}

int ConstructionGroup::placeItem(int x, int y, unsigned short type)
{
    Construction *tmpConstr = createConstruction(x, y, type);

    if (tmpConstr == NULL)
    {
        std::cout << "failed to createConstruction " << type << std::endl;
        return -1;
    }

    if (world(x, y)->construction) //no two constructions at the same mapTile
    {	do_bulldoze_area(x, y);}
    unsigned short size = 0;
    world(x, y)->construction = tmpConstr;
    constructionCount.add_construction(tmpConstr); //register for Simulation
    size = (world(x, y)->construction->constructionGroup->size);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            world(x + j, y + i)->reportingConstruction = world(x, y)->construction;
            if (!world(x + j, y + i)->is_water())
            {
                world(x + j, y + i)->setTerrain(CST_DESERT);
            } // endif !is_water
        } //endfor j
    }// endfor i

    return 0;
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





//Legacy Stuff


struct TYPE main_types[NUM_OF_TYPES];

/* AL1 : 2.x : RANGE of buildings is not included in this struct GROUP
 *
 * 		this is a bit annoying => would need to be fixed when(if?)
 * 		all the struct and associated macros are cleaned (map, MP_INFO etc.)
 *
 * 		for now (2.1 svn rev 1585) just put it in range.h
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
     1,                         /* size */
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
     4,                         /* size */
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
     2,                         /* size */
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
     3,                         /* size */
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
     4,                         /* size */
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
     2,                         /* size */
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
     1,                         /* size */
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
     4,                         /* size */
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
     1,                         /* size */
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
     4,                         /* size */
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
     1,                         /* size */
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
     3,                         /* size */
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
     3,                         /* size */
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
     4,                         /* size */
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
     4,                         /* size */
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
     4,                         /* size */
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
     4,                         /* size */
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
     4,                         /* size */
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
     1,                         /* size */
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
     2,                         /* size */
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
     1,                         /* size */
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
     2,                         /* size */
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
     4,                         /* size */
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
     2,                         /* size */
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
     2,                         /* size */
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
     2,                         /* size */
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
     2,                         /* size */
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
     2,                         /* size */
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
     2,                         /* size */
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
     2,                         /* size */
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
     2,                         /* size */
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
     2,                         /* size */
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
     1,                         /* size */
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
     3,                         /* size */
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
     3,                         /* size */
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
     3,                         /* size */
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
     3,                         /* size */
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
     3,                         /* size */
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
     2,                         /* size */
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
     1,                         /* size */
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
     1,                         /* size */
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
     1,                         /* size */
     GROUP_RAIL_BRIDGE_COLOUR,
     GROUP_RAIL_BRIDGE_COST_MUL,
     GROUP_RAIL_BRIDGE_BUL_COST,
     GROUP_RAIL_BRIDGE_FIREC,
     GROUP_RAIL_BRIDGE_COST,
     GROUP_RAIL_BRIDGE_TECH},

     /* 49 */
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

int get_group_of_type(short type)
{
    if (type == CST_NONE)
        return GROUP_BARE;
    return main_types[type].group;
}

void set_map_groups(void)
{
    int x, y;
    for (x = 0; x < WORLD_SIDE_LEN; x++) {
        for (y = 0; y < WORLD_SIDE_LEN; y++) {
            world(x,y)->group = get_group_of_type(world(x,y)->type);
        }
    }
}

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

/** @file lincity/lintypes.cpp */

