#include "ConstructionRequest.h"

#include "lintypes.h"
#include "lin-city.h"
#include "lctypes.h"
#include "engglobs.h"
#include "engine.h"
#include "simulate.h"
#include "transport.h"
#include "../gui_interface/mps.h"
#include "modules/all_modules.h"
#include "all_buildings.h"

//#include "../lincity-ng/Mps.hpp"
//FIXME cannot include mps.h because of differing paths for further dependencies
extern int mps_x, mps_y;

void ConstructionDeletionRequest::execute()
{
    int size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    ::constructionCount.remove_construction(subject);
    world(x,y)->construction = NULL;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            world(x+j,y+i)->reportingConstruction = NULL;
            //update mps display            
            if (mps_x == x && mps_y == y)
            {
                mps_set(x, y, MPS_MAP);
            }
        }
    }
    
    delete subject;

    // update adjacencies
    desert_frontier(x - 1, y - 1, size + 2, size + 2); 
    connect_rivers();
    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1); 
}

void OreMineDeletionRequest::execute()
{
    int size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    ::constructionCount.remove_construction(subject);
    world(x,y)->construction = NULL;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
           world(x+j,y+i)->reportingConstruction = NULL;
            if (world(x+j,y+i)->ore_reserve < ORE_RESERVE / 2)
            {
                world(x+j,y+i)->setTerrain(CST_WATER);                
                world(x+j,y+i)->flags |= FLAG_HAS_UNDERGROUND_WATER;
            }            
            //update mps display            
            if (mps_x == x && mps_y == y)
            {
                mps_set(x, y, MPS_MAP);
            }
        }
    }
    
    delete subject;

    // update adjacencies
    desert_frontier(x - 1, y - 1, size + 2, size + 2); 
    connect_rivers();
    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1); 
}

void CommuneDeletionRequest::execute()
{
    int size = subject->constructionGroup->size;
    int x = subject->x;
    int y = subject->y;
    ::constructionCount.remove_construction(subject);
    world(x,y)->construction = NULL;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            world(x+j,y+i)->reportingConstruction = NULL;
            if (world(x+j,y+i)->flags & FLAG_HAS_UNDERGROUND_WATER)
                parklandConstructionGroup.placeItem(x+j, y+i, CST_PARKLAND_PLANE);            
            //update mps display            
            if (mps_x == x && mps_y == y)
            {
                mps_set(x, y, MPS_MAP);
            }
        }
    }
    
    delete subject;

    // update adjacencies
    desert_frontier(x - 1, y - 1, size + 2, size + 2); 
    connect_rivers();
    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1); 
}

void PowerLineFlashRequest::execute()
{    
    int *anim_counter = &(dynamic_cast<Powerline*>(subject)->anim_counter);
    // 2/3 cooldown will prevent interlacing wave packets    
    if (*anim_counter <= POWER_MODULUS/3)  
    {     
        *anim_counter = POWER_MODULUS;
    }      
}
