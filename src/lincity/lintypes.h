/* ---------------------------------------------------------------------- *
 * lintypes.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lintypes_h__
#define __lintypes_h__

#define WORLD_SIDE_LEN 100
#define OLD_MAX_NUMOF_SUBSTATIONS 100
#define MAX_NUMOF_SUBSTATIONS 512

#define NUMOF_COAL_RESERVES ((world.len() * world.len()) / 400)
#define NUM_OF_TYPES    404
#define NUM_OF_GROUPS    51

#define OLD_MAX_NUMOF_MARKETS 100
#define MAX_NUMOF_MARKETS 512

#define NUMOF_DAYS_IN_MONTH 100
#define NUMOF_DAYS_IN_YEAR (NUMOF_DAYS_IN_MONTH*12)

#define NUMOF_DISCOUNT_TRIGGERS 6

//pages for report 0,1,2,3,...,LAST_REPORT_PAGE
#define LAST_REPORT_PAGE 3

/*

int get_group_cost(short group);
int get_type_cost(short type);
void get_type_name(short type, char *s);
*/
unsigned short get_group_of_type(unsigned short selected_type);
void set_map_groups(void);
/********** Data structures ***************/
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <sstream>
#include <zlib.h>
#include "ConstructionCount.h"
#include "engglobs.h"
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_image.h>
#include "gui/Texture.hpp"
class Construction;

// Class to count instanced objects of each construction type

template <typename Class>
class Counted
{
public:
    Counted()
    {
        instanceCount++;
        nextId++;
    }
    ~Counted()
    {
        --instanceCount;
        //reset unique only Id after the last Construction is gone
        if (instanceCount == 0)
        { nextId = 0;}
    }
    static unsigned int getInstanceCount() {
        return instanceCount;
    }
    static unsigned int getNextId() {
        return nextId;
    }
private:
    static unsigned int instanceCount, nextId;
};


template <typename Class>
unsigned int Counted<Class>::instanceCount;
template <typename Class>
unsigned int Counted<Class>::nextId;

class Ground
{
public:
    Ground();
    ~Ground();
    int altitude;       //surface of ground. unused currently
    int ecotable;       //done at init time: pointer to the table for vegetation
    int wastes;         //wastes underground
    int pollution;      //pollution underground
    int water_alt;      //altitude of water (needed to know drainage basin)
    int water_pol;      //pollution of water
    int water_wast;     //wastes in water
    int water_next;     //next tile(s) where the water will go from here
    int int1;           //reserved for future (?) use
    int int2;
    int int3;
    int int4;
};

class ConstructionGroup;

class MapTile {
public:
    MapTile();
    ~MapTile();
    Ground ground;                        //the Ground associated to an instance of MapTile
    Construction *construction;           //the actual construction (e.g. for simulation)
    Construction *reportingConstruction;  //the construction covering the tile
    unsigned short type;                  //type of terrain (underneath constructions)
    unsigned short group;                 //group of the terrain (underneath constructions)
    int flags;                            //flags are defined in lin-city.h
    unsigned short coal_reserve;          //underground coal
    unsigned short ore_reserve;           //underground ore
    int pollution;                        //air pollution (under ground pollution is in ground[][])


    void setTerrain(unsigned short group); //places type & group at MapTile
    unsigned short getType();          //type of bare land or the covering construction
    unsigned short getTopType();       //type of bare land or the actual construction
    unsigned short getGroup();        //group of bare land or the covering construction
    unsigned short getTopGroup();     //group of bare land or the actual construction
    unsigned short getTransportGroup(); //like getGroup but bridges are reported normal transport tiles
    ConstructionGroup* getTileConstructionGroup(); //constructionGroup of the maptile
    ConstructionGroup* getConstructionGroup();     //constructionGroup of maptile or the covering construction
    ConstructionGroup* getTopConstructionGroup();  //constructionGroup of maptile or the actual construction

    bool is_bare();                    //true if we there is neither a covering construction nor water
    bool is_lake();                    //true on lakes (also under bridges)
    bool is_river();                   //true on rivers (also under bridges)
    bool is_water();                   //true on bridges or lakes (also under bridges)
    bool is_visible();                 //true if tile is not covered by another construction. Only useful for minimap Gameview is rotated to upperleft
    bool is_transport();               //true on tracks, road, rails and bridges
    bool is_powerline();               //true on power lines
    bool is_residence();               //true if any residence covers the tile
    void writeTemplate();              //create maptile template
    void saveMembers(std::ostream *os);//write maptile AND ground members as XML to stram
};


class MemberRule{
public:
    int memberType; //type of ConstructionMember
    void *ptr; //address of ConstructionMember
};

template <typename MemberType>
class MemberTraits { };

struct CommodityRule{
    int maxload;
    bool take;
    bool give;
};

class Construction {
public:
    virtual ~Construction() {}
    virtual void update() = 0;
    virtual void report() = 0;


    ConstructionGroup *constructionGroup;
    unsigned short type;
    int x, y;
    int ID;
    int flags;              //flags are defined in lin-city.h

    enum Commodities
    {
        STUFF_FOOD,
        STUFF_JOBS,
        STUFF_COAL,
        STUFF_GOODS,
        STUFF_ORE,
        STUFF_STEEL,
        STUFF_WASTE,
        STUFF_KWH,
        STUFF_MWH,
        STUFF_WATER
    };

    enum MemberTypes
    {
        TYPE_BOOL,
        TYPE_INT,
        TYPE_USHORT,
        TYPE_DOUBLE,
        TYPE_FLOAT
    };

    std::map<Commodities, int> commodityCount;  //map that holds all kinds of stuff
    std::map<std::string, MemberRule> memberRuleCount;
    std::vector<Construction*> neighbors;       //adjacent for transport
    std::vector<Construction*> partners;        //remotely for markets

    void list_commodities(int *);                   //prints a sorted list all commodities in report()
    void report_commodities(void);                  //adds commodities and capacities to gloabl stat counter
    //void list_connections(int *);                 //prints the detected lurd connections
    void initialize_commodities(void);              //sets all commodities to 0 and marks them as saved members
    void bootstrap_commodities(int percentage);     // sets all commodities except STUFF_WASTE to percentage.
    int loadMember(std::string const &xml_tag, std::string const &xml_val);
    int readbinaryMember(std::string const &xml_tag, gzFile fp);
    template <typename MemberType>
    void setMemberSaved(MemberType *ptr, std::string const &xml_tag)
    {
        memberRuleCount[xml_tag].memberType = MemberTraits<MemberType>::TYPE_ID;
        memberRuleCount[xml_tag].ptr = static_cast<void *>(ptr);
    }

    void setCommodityRulesSaved(std::map<Commodities,CommodityRule> * stuffRuleCount);
    void writeTemplate();      //create xml template for savegame
    void saveMembers(std::ostream *os);        //writes all needed and optionally set Members as XML to stream
    void detach();      //removes all references from world, ::constructionCount
    void deneighborize(); //cancells all neighbors and partners mutually
    void   neighborize(); //adds all neigborconnections once (call twice for double connections)
    void link_to(Construction* other); //establishes mutual connection to neighbor or partner
    int  tellstuff( Commodities stuff_ID, int level); //tell the filling level of commodity
    void trade(); //exchange commodities with neigbhors
    int equilibrate_stuff(int *rem_lvl, int rem_cap , int ratio, Commodities stuff_ID, ConstructionGroup * rem_cstGroup);
    //equilibrates stuff with an external reservoir (e.g. another construction invoking this method)
    void playSound();//plays random chunk from constructionGroup
};

extern const char *commodityNames[];
//global Vars for statistics on commodities
extern std::map<Construction::Commodities, int> tstat_capacities;
extern std::map<Construction::Commodities, int> tstat_census;

#define MEMBER_TYPE_TRAITS(MemberType, TypeId) \
template <> \
class MemberTraits<MemberType> { \
public: \
    enum { TYPE_ID = TypeId }; \
};

MEMBER_TYPE_TRAITS(int, Construction::TYPE_INT)
MEMBER_TYPE_TRAITS(bool, Construction::TYPE_BOOL)
MEMBER_TYPE_TRAITS(unsigned short, Construction::TYPE_USHORT)
MEMBER_TYPE_TRAITS(double, Construction::TYPE_DOUBLE)
MEMBER_TYPE_TRAITS(float, Construction::TYPE_FLOAT)

class ConstructionGroup;

template <typename ConstructionClass>
class RegisteredConstruction: public Construction, public Counted<ConstructionClass>
{
public:
    RegisteredConstruction<ConstructionClass>( int x, int y)
    {
        this->type = 0;//safe default
        setMemberSaved(&(this->type),"type");
        this->x = x;
        this->y = y;
        this->ID = Counted<ConstructionClass>::getNextId();
        this->flags = '\0';
        setMemberSaved(&(this->flags),"flags");
#ifdef DEBUG
        neighbors.clear();
        partners.clear();
#endif
    }
    ~RegisteredConstruction<ConstructionClass>(){}
};

class GraphicsInfo
{
    public:
    GraphicsInfo(void){
        texture = (Texture*)'\0';
        image = (SDL_Surface*)'\0';
        x = 0;
        y = 0;
    }

    Texture* texture;
    SDL_Surface* image;
    int x, y;
};

class ConstructionGroup {
public:
    ConstructionGroup(
        const char *name,
        bool no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ) {
        this->name = name;
        this->no_credit = no_credit;
        this->group = group;
        this->size = size;
        this->colour = colour;
        this->cost_mul = cost_mul;
        this->bul_cost = bul_cost;
        this->fire_chance = fire_chance;
        this->cost = cost;
        this->tech = tech;
        this->range = range;
        this->images_loaded = false;
        this->sounds_loaded = false;
       }
    ~ConstructionGroup()
    {
        std::vector<GraphicsInfo>::iterator it;
        for(it = graphicsInfoVector.begin(); it != graphicsInfoVector.end(); ++it)
        {
            if(it->texture)
            {
                delete it->texture;
                it->texture = 0;
            }
            //CK it seems that SDL images are freed
            //on their own, crashes rarely
/*
            else if (it->image)
            {
                SDL_FreeSurface(it->image);
                it->image = 0;
            }
*/
        }
    }


    std::map<Construction::Commodities, CommodityRule> commodityRuleCount;
    std::vector<Mix_Chunk *> chunks;
    std::vector<GraphicsInfo> graphicsInfoVector;
    int getCosts();
    bool is_allowed_here(int x, int y, bool msg);//check if construction could be placed
    void growGraphicsInfoVector(void);

    virtual int placeItem(int x, int y);

    // this method must be overriden by the concrete ConstructionGroup classes.
    virtual Construction *createConstruction(int x, int y) = 0;

    std::string resourceID;           /* name for matching resources from XML*/
    const char *name;           /* inGame name of group */
    bool no_credit;   /* TRUE if need credit to build */
    unsigned short group;       /* This is redundant: it must match
                                   the index into the table */
    unsigned short size;        /* shape in x and y */
    int colour;                 /* summary map colour */
    int cost_mul;               /* group cost multiplier */
    int bul_cost;               /* group bulldoze cost */
    int fire_chance;            /* probability of fire */
    int cost;                   /* group cost */
    int tech;                   /* group tech */
    int range;                  /* range beyond size*/
    bool images_loaded;
    bool sounds_loaded;

    static void addConstructionGroup(ConstructionGroup *constructionGroup)
    {
        if ( groupMap.count(constructionGroup->group) )
        {
            std::cout << "rejecting " << constructionGroup->name << " as "
            << constructionGroup->group << " from ConstructionGroup::groupMap"
            << std::endl;
        }
        else
        {   groupMap[constructionGroup->group] = constructionGroup;}

    }

    static void addResourceID(std::string resID, ConstructionGroup *constructionGroup)
    {

        if ( resourceMap.count(resID))
        {
            std::cout << "rejecting " << constructionGroup->name << " as "
            << constructionGroup->resourceID << " from ConstructionGroup::resouceMap"
            << std::endl;
            }
        else
        {
            constructionGroup->resourceID = resID;
            resourceMap[constructionGroup->resourceID] = constructionGroup;
        }
    }

    static void clearGroupMap()
    {
        // removes all entries from groupMap
        groupMap.clear();
    }

    static void clearResourcepMap()
    {
        // removes all entries from resourceMap
        resourceMap.clear();
    }


    static ConstructionGroup *getConstructionGroup(unsigned short group)
    {
        if (groupMap.count(group))
            return groupMap[group];
        else
            return NULL;
    }

    static void printGroups();

    static int countConstructionGroup(unsigned short group) {
        return groupMap.count(group);
    }

    static std::map<std::string, ConstructionGroup*> resourceMap;
protected:
    // Map associating group ids with the respective construction group objects
    static std::map<unsigned short, ConstructionGroup*> groupMap;

};

struct GROUP {
    const char *name;           // name of group
    unsigned short no_credit;   // TRUE if need credit to build
    unsigned short group;       // This is redundant: it must match
                                // the index into the table
    unsigned short size;
    int colour;                 // summary map colour
    int cost_mul;               // group cost multiplier
    int bul_cost;               // group bulldoze cost
    int fire_chance;            // probability of fire
    int cost;                   // group cost
    int tech;                   // group tech
};


struct TYPE {
    int group;                  // What group does this type belong to?
    char *graphic;              // Bitmap of the graphic
};


#endif /* __lintypes_h__ */

/** @file lincity/lintypes.h */

