#define GROUP_INDUSTRY_L_COLOUR (cyan(18))
#define GROUP_INDUSTRY_L_COST 20000
#define GROUP_INDUSTRY_L_COST_MUL 25
#define GROUP_INDUSTRY_L_BUL_COST 20000
#define GROUP_INDUSTRY_L_TECH 160
#define GROUP_INDUSTRY_L_FIREC 70
#define GROUP_INDUSTRY_L_RANGE 0
#define GROUP_INDUSTRY_L_SIZE 3

#define INDUSTRY_L_ORE_USED  125
#define INDUSTRY_L_STEEL_USED  12
#define INDUSTRY_L_JOBS_USED   30
#define INDUSTRY_L_JOBS_LOAD_ORE 1
#define INDUSTRY_L_JOBS_LOAD_STEEL 4
#define MIN_JOBS_AT_INDUSTRY_L (INDUSTRY_L_JOBS_LOAD_ORE + INDUSTRY_L_JOBS_LOAD_STEEL + INDUSTRY_L_JOBS_USED)
#define INDUSTRY_L_MAKE_GOODS 225
#define INDUSTRY_L_POWER_PER_GOOD 10

#define MAX_ORE_AT_INDUSTRY_L (20 * INDUSTRY_L_ORE_USED)
#define MAX_JOBS_AT_INDUSTRY_L (20 * MIN_JOBS_AT_INDUSTRY_L)
#define MAX_GOODS_AT_INDUSTRY_L (20*2*4 * INDUSTRY_L_MAKE_GOODS)
#define MAX_WASTE_AT_INDUSTRY_L (MAX_GOODS_AT_INDUSTRY_L / 20)
#define MAX_KWH_AT_INDUSTY_L (INDUSTRY_L_POWER_PER_GOOD*MAX_GOODS_AT_INDUSTRY_L)
#define MAX_MWH_AT_INDUSTY_L (INDUSTRY_L_POWER_PER_GOOD*MAX_GOODS_AT_INDUSTRY_L / 2)
#define MAX_STEEL_AT_INDUSTRY_L (20 * INDUSTRY_L_STEEL_USED)

#define INDUSTRY_L_ANIM_SPEED 290
#define INDUSTRY_L_POL_PER_GOOD 0.05

class IndustryLightConstructionGroup: public ConstructionGroup {
public:
    IndustryLightConstructionGroup(
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
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_INDUSTRY_L;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_GOODS].maxload = MAX_GOODS_AT_INDUSTRY_L;
        commodityRuleCount[Construction::STUFF_GOODS].take = false;
        commodityRuleCount[Construction::STUFF_GOODS].give = true;
        commodityRuleCount[Construction::STUFF_ORE].maxload = MAX_ORE_AT_INDUSTRY_L;
        commodityRuleCount[Construction::STUFF_ORE].take = true;
        commodityRuleCount[Construction::STUFF_ORE].give = false;
        commodityRuleCount[Construction::STUFF_STEEL].maxload = MAX_STEEL_AT_INDUSTRY_L;
        commodityRuleCount[Construction::STUFF_STEEL].take = true;
        commodityRuleCount[Construction::STUFF_STEEL].give = false;
        commodityRuleCount[Construction::STUFF_WASTE].maxload = MAX_WASTE_AT_INDUSTRY_L;
        commodityRuleCount[Construction::STUFF_WASTE].take = false;
        commodityRuleCount[Construction::STUFF_WASTE].give = true;
        commodityRuleCount[Construction::STUFF_KWH].maxload = MAX_KWH_AT_INDUSTY_L;
        commodityRuleCount[Construction::STUFF_KWH].take = true;
        commodityRuleCount[Construction::STUFF_KWH].give = false;
        commodityRuleCount[Construction::STUFF_MWH].maxload = MAX_MWH_AT_INDUSTY_L;
        commodityRuleCount[Construction::STUFF_MWH].take = true;
        commodityRuleCount[Construction::STUFF_MWH].give = false;
    };
    // overriding method that creates a LightIndustry
    virtual Construction *createConstruction(int x, int y);
};

extern IndustryLightConstructionGroup industryLightConstructionGroup;
extern IndustryLightConstructionGroup industryLight_Q_ConstructionGroup;
extern IndustryLightConstructionGroup industryLight_L_ConstructionGroup;
extern IndustryLightConstructionGroup industryLight_M_ConstructionGroup;
extern IndustryLightConstructionGroup industryLight_H_ConstructionGroup;


class IndustryLight: public RegisteredConstruction<IndustryLight> { // IndustryLight inherits from RegisteredConstruction
public:
    IndustryLight(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<IndustryLight>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        this->working_days = 0;
        this->busy = 0;
        this->goods_this_month = 0;
        this->anim = 0;
        initialize_commodities();
        this->bonus = 0;
        setMemberSaved(&this->bonus, "bonus");
        this->extra_bonus = 0;
        setMemberSaved(&this->extra_bonus, "extra_bonus");
        if (tech > MAX_TECH_LEVEL)
        {
            bonus = (tech - MAX_TECH_LEVEL);
            if (bonus > MAX_TECH_LEVEL)
                bonus = MAX_TECH_LEVEL;
            bonus /= MAX_TECH_LEVEL;
            // check for filter technology bonus
            if (tech > 2 * MAX_TECH_LEVEL)
            {
                extra_bonus = tech - 2 * MAX_TECH_LEVEL;
                if (extra_bonus > MAX_TECH_LEVEL)
                    extra_bonus = MAX_TECH_LEVEL;
                extra_bonus /= MAX_TECH_LEVEL;
            }
        }
    }
    virtual void update();
    virtual void report();

    int  tech;
    double bonus, extra_bonus;
    int  working_days;
    int  busy;
    int  anim;
    int  goods_this_month;
};


/** @file lincity/modules/light_industry.h */

