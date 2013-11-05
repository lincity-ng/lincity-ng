#define GROUP_FIRE_COLOUR    (red(30))
#define GROUP_FIRE_COST   0     /* Unused */
#define GROUP_FIRE_COST_MUL   1 /* Unused */
#define GROUP_FIRE_BUL_COST  1000
#define GROUP_FIRE_TECH   0     /* Unused */
#define GROUP_FIRE_FIREC  0
#define GROUP_FIRE_RANGE  0
#define GROUP_FIRE_SIZE   1

#define DAYS_BETWEEN_FIRES (NUMOF_DAYS_IN_YEAR*2)
#define FIRE_ANIMATION_SPEED (200 + rand()%350 -175)
#define FIRE_DAYS_PER_SPREAD (NUMOF_DAYS_IN_YEAR/8)
#define FIRE_LENGTH (NUMOF_DAYS_IN_YEAR*5)
#define AFTER_FIRE_LENGTH (NUMOF_DAYS_IN_YEAR*10)

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"
#include <stdlib.h>


class FireConstructionGroup: public ConstructionGroup {
public:
    FireConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech, range
    ) {

    };
    // overriding method that creates a Fire
    virtual Construction *createConstruction(int x, int y);
};

extern FireConstructionGroup fireConstructionGroup;
extern FireConstructionGroup fireWasteLandConstructionGroup;

class Fire: public RegisteredConstruction<Fire> { // Fire inherits from Construction
public:
    Fire(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Fire>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->burning_days = 0;
        setMemberSaved(&this->burning_days, "burning_days");
        this->smoking_days = 0;
        setMemberSaved(&this->smoking_days, "smoking_days");
        this->anim = 0;
        this->days_before_spread = FIRE_DAYS_PER_SPREAD;
        setMemberSaved(&this->days_before_spread, "days_before_spread");
    }
    virtual void update();
    virtual void report();

    int burning_days;
    int smoking_days;
    int days_before_spread;
    int anim;
};

/** @file lincity/modules/fire.h */

