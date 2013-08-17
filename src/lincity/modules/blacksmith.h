#define GROUP_BLACKSMITH_COLOUR (white(15))
#define GROUP_BLACKSMITH_COST   5000
#define GROUP_BLACKSMITH_COST_MUL 25
#define GROUP_BLACKSMITH_BUL_COST   1000
#define GROUP_BLACKSMITH_TECH   3
#define GROUP_BLACKSMITH_FIREC 60
#define GROUP_BLACKSMITH_RANGE 0
#define GROUP_BLACKSMITH_SIZE 2

#define BLACKSMITH_JOBS    35
#define BLACKSMITH_GET_COAL 6

#define BLACKSMITH_STEEL_USED 1
#define BLACKSMITH_COAL_USED  1
#define GOODS_MADE_BY_BLACKSMITH 50
#define MAX_JOBS_AT_BLACKSMITH (BLACKSMITH_JOBS*20)
#define MAX_COAL_AT_BLACKSMITH (BLACKSMITH_COAL_USED*20)
#define MAX_STEEL_AT_BLACKSMITH (BLACKSMITH_STEEL_USED*20)
#define MAX_GOODS_AT_BLACKSMITH (GOODS_MADE_BY_BLACKSMITH*20)
#define BLACKSMITH_CLOSE_TIME 25

#define BLACKSMITH_BATCH (GOODS_MADE_BY_BLACKSMITH*100)
#define BLACKSMITH_ANIM_THRESHOLD 10
#define BLACKSMITH_ANIM_SPEED    200

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"

class BlacksmithConstructionGroup: public ConstructionGroup {
public:
    BlacksmithConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech, range
    )
    {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_BLACKSMITH;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_COAL].maxload = MAX_COAL_AT_BLACKSMITH;
        commodityRuleCount[Construction::STUFF_COAL].take = true;
        commodityRuleCount[Construction::STUFF_COAL].give = false;
        commodityRuleCount[Construction::STUFF_STEEL].maxload = MAX_STEEL_AT_BLACKSMITH;
        commodityRuleCount[Construction::STUFF_STEEL].take = true;
        commodityRuleCount[Construction::STUFF_STEEL].give = false;
        commodityRuleCount[Construction::STUFF_GOODS].maxload = MAX_GOODS_AT_BLACKSMITH;
        commodityRuleCount[Construction::STUFF_GOODS].take = false;
        commodityRuleCount[Construction::STUFF_GOODS].give = true;         
    }
    // overriding method that creates a blacksmith
    virtual Construction *createConstruction(int x, int y, unsigned short type);
};

extern BlacksmithConstructionGroup blacksmithConstructionGroup;

class Blacksmith: public CountedConstruction<Blacksmith> { // Blacksmith inherits from its CountedConstruction
public:
	Blacksmith(int x, int y, unsigned short type): CountedConstruction<Blacksmith>(x, y, type)  
    {
        //static int blacksmithID = 0;        
        constructionGroup = &blacksmithConstructionGroup;		
        //this->x = x; this->y = y;
		//this->type = CST_BLACKSMITH_0;
        //this->ID = ++blacksmithID;
        //this->flags = 0;        
        this->anim = 0; // or real_time?
        this->pauseCounter = 0;        
        this->productivity = 0;
        this->workingdays = 0;        
        this->animate = false;
        this->goods_made = 0;
        initialize_commodities();
        //this->commodityCount[STUFF_JOBS] = 0;
        //this->commodityCount[STUFF_COAL] = 0;
        //this->commodityCount[STUFF_STEEL] = 0;         
        //this->commodityCount[STUFF_GOODS] = 0;
                     
	}
	virtual ~Blacksmith() { }
	virtual void update();
	virtual void report();
    
    int  goods_made;    
    int  anim;    
    int  pauseCounter;
    int  workingdays;
    int  productivity;
    bool animate;	
};


/** @file lincity/modules/blacksmith.h */

