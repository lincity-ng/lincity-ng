#define GROUP_RECYCLE_COLOUR   (green(28))
#define GROUP_RECYCLE_COST    100000
#define GROUP_RECYCLE_COST_MUL 5
#define GROUP_RECYCLE_BUL_COST    1000
#define GROUP_RECYCLE_TECH    232
#define GROUP_RECYCLE_FIREC 10
#define GROUP_RECYCLE_RANGE 0
#define GROUP_RECYCLE_SIZE 2

#define WASTE_RECYCLED       500
#define RECYCLE_LABOR   (WASTE_RECYCLED/50 + LABOR_LOAD_ORE + LABOR_LOAD_STEEL)
#define RECYCLE_RUNNING_COST 3
#define LOVOLT_RECYCLE_WASTE (WASTE_RECYCLED/2)

#define MAX_LABOR_AT_RECYCLE (20 * RECYCLE_LABOR)
#define MAX_WASTE_AT_RECYCLE (20 * WASTE_RECYCLED)
#define MAX_ORE_AT_RECYCLE (16 * WASTE_RECYCLED)
#define MAX_LOVOLT_AT_RECYCLE   (20 * LOVOLT_RECYCLE_WASTE)
#define MAX_STEEL_AT_RECYCLE (16 * WASTE_RECYCLED/50)

#define BURN_WASTE_AT_RECYCLE (MAX_WASTE_AT_RECYCLE/200)



#include <array>                    // for array
#include <string>                   // for basic_string

#include "modules.h"

class RecycleConstructionGroup: public ConstructionGroup {
public:
    RecycleConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    )
    {
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_RECYCLE;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_RECYCLE;
        commodityRuleCount[STUFF_WASTE].take = true;
        commodityRuleCount[STUFF_WASTE].give = false;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_AT_RECYCLE;
        commodityRuleCount[STUFF_LOVOLT].take = true;
        commodityRuleCount[STUFF_LOVOLT].give = false;
        commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_AT_RECYCLE;
        commodityRuleCount[STUFF_STEEL].take = false;
        commodityRuleCount[STUFF_STEEL].give = true;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_AT_RECYCLE;
        commodityRuleCount[STUFF_ORE].take = false;
        commodityRuleCount[STUFF_ORE].give = true;
    }
    // overriding method that creates a recyle
    virtual Construction *createConstruction(int x, int y);
};

extern RecycleConstructionGroup recycleConstructionGroup;

class Recycle: public RegisteredConstruction<Recycle> { // Recycle inherits from Construction
public:
    Recycle(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Recycle>(x, y)
    {
        this->constructionGroup = cstgrp;
        init_resources();
        this->busy = 0;
        this->working_days = 0;
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        initialize_commodities();
        // int efficiency;
        // efficiency = ( WASTE_RECYCLED * (10 + ( (50 * tech) / MAX_TECH_LEVEL)) ) / 100;
        // if (efficiency > (WASTE_RECYCLED * 8) / 10)
        // {   efficiency = (WASTE_RECYCLED * 8) / 10;}
        // this->make_ore = efficiency;
        setMemberSaved(&this->make_ore, "make_ore"); // compatibility
        // this->make_steel = efficiency / 50;
        setMemberSaved(&this->make_steel, "make_steel"); // compatibility

        commodityMaxCons[STUFF_LABOR] = 100 * RECYCLE_LABOR;
        commodityMaxCons[STUFF_LOVOLT] = 100 * LOVOLT_RECYCLE_WASTE;
        commodityMaxCons[STUFF_WASTE] = 100 *
          (WASTE_RECYCLED + BURN_WASTE_AT_RECYCLE);
        // commodityMaxProd[STUFF_ORE] = 100 * make_ore;
        // commodityMaxProd[STUFF_STEEL] = 100 * make_steel;
    }

    virtual void initialize() override {
        RegisteredConstruction::initialize();


        int efficiency =
          (WASTE_RECYCLED * (10 + ((50 * tech) / MAX_TECH_LEVEL))) / 100;
        if (efficiency > (WASTE_RECYCLED * 8) / 10)
        {   efficiency = (WASTE_RECYCLED * 8) / 10;}
        this->make_ore = efficiency;
        this->make_steel = efficiency / 50;

        commodityMaxProd[STUFF_ORE] = 100 * make_ore;
        commodityMaxProd[STUFF_STEEL] = 100 * make_steel;
    }

    virtual ~Recycle() { }
    virtual void update() override;
    virtual void report() override;

    int  tech;
    int  make_ore;
    int  make_steel;
    int  working_days, busy;
};

/** @file lincity/modules/recycle.h */
