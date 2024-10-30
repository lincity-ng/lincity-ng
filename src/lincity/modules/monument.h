#define GROUP_MONUMENT_COLOUR (white(15))
#define GROUP_MONUMENT_COST   10000
#define GROUP_MONUMENT_COST_MUL 25
#define GROUP_MONUMENT_BUL_COST   1000000
#define GROUP_MONUMENT_TECH   0
#define GROUP_MONUMENT_FIREC  0
#define GROUP_MONUMENT_RANGE  0
#define GROUP_MONUMENT_SIZE  2

#define BUILD_MONUMENT_LABOR     350000
#define MONUMENT_GET_LABOR       100
#define MAX_LABOR_AT_MONUMENT    (MONUMENT_GET_LABOR*20)
#define MONUMENT_DAYS_PER_TECH  3
#define MONUMENT_TECH_EXPIRE    400
//#define MONUMENT_ANIM_SPEED     300 //actually used?

#include <array>                    // for array
#include <string>                   // for basic_string

#include "modules.h"

class MonumentConstructionGroup: public ConstructionGroup {
public:
    MonumentConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    ) {
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_MONUMENT;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;

    }
    // overriding method that creates a monument
    virtual Construction *createConstruction();
};

extern MonumentConstructionGroup monumentConstructionGroup;

class Monument: public Construction {
public:
    Monument(ConstructionGroup *cstgrp) {
        this->constructionGroup = cstgrp;
        init_resources();
        this->busy = 0;
        this->working_days = 0;
        this->tech_made = 0;
        this->tail_off = 0;
        this->completion = 0;
        this->completed = false; //don't save this one
        this->labor_consumed = 0;
        initialize_commodities();

        commodityMaxCons[STUFF_LABOR] = 100 * MONUMENT_GET_LABOR;
    }

    virtual ~Monument() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    virtual void save(xmlTextWriterPtr xmlWriter) override;
    virtual bool loadMember(xmlpp::TextReader& xmlReader) override;

    int  working_days, busy;
    int  tech_made;
    int  tail_off;
    int  completion;
    bool completed;
    int  labor_consumed;
};

/** @file lincity/modules/monument.h */
