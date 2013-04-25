/* ---------------------------------------------------------------------- *
 * power_line.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */
#include "power_line.h"

//Power line
PowerlineConstructionGroup powerlineConstructionGroup(
    "Power line",
    FALSE,                     /* need credit? */
    GROUP_POWER_LINE,
    1,                         /* size */
    GROUP_POWER_LINE_COLOUR,
    GROUP_POWER_LINE_COST_MUL,
    GROUP_POWER_LINE_BUL_COST,
    GROUP_POWER_LINE_FIREC,
    GROUP_POWER_LINE_COST,
    GROUP_POWER_LINE_TECH
);

Construction *PowerlineConstructionGroup::createConstruction(int x, int y, unsigned short type)
{
    return new Powerline(x, y, type);
}

void Powerline::flow_power()
{
    int ratio, center_ratio, center_lvl, center_cap, xx, yy, traffic;
    int left_ratio, right_ratio, up_ratio, down_ratio, n;
    bool far_left, far_right, far_up, far_down;
    Commodities stuff_ID = STUFF_MWH;
    
    center_lvl = commodityCount[stuff_ID];
    center_cap = constructionGroup->commodityRuleCount[stuff_ID].maxload;
    far_left = false;
    far_right = false;
    far_up = false;
    far_down = false;
    left_ratio = -1;
    right_ratio = -1;
    up_ratio = -1;
    down_ratio = -1;    
    center_ratio = (center_lvl * TRANSPORT_QUANTA / (center_cap) );
    /*see how much stuff is there around*/    
    if (flags & FLAG_RIGHT) //This is consistent to connect_transport()
    {                 
        left_ratio = collect_transport_info(x-1 ,y , stuff_ID, center_ratio );//left
        if (left_ratio == -1)
        {
            left_ratio = collect_transport_info(x-2 ,y , stuff_ID, center_ratio );//far left
            far_left = true;
       }
    }    
    if (flags & FLAG_UP) //This is consistent to connect_transport()
    {                 
        right_ratio = collect_transport_info(x+1 ,y , stuff_ID, center_ratio );//right
        if (right_ratio == -1)
        {
            right_ratio = collect_transport_info(x+2 ,y , stuff_ID, center_ratio );//far right
            far_right = true;
        }
    }       
    if (flags & FLAG_LEFT) //This is consistent to connect_transport()
    {                 
        up_ratio = collect_transport_info(x , y+1 , stuff_ID, center_ratio );//up
        if (up_ratio == -1)
        {
            up_ratio = collect_transport_info(x , y+2 , stuff_ID, center_ratio );//far up
            far_up = true;
        }
    }    
    if (flags & FLAG_DOWN) //This is ALSO consistent to connect_transport()
    {                 
        down_ratio = collect_transport_info(x , y-1 , stuff_ID, center_ratio );//down
        if (down_ratio == -1)
        {
            down_ratio = collect_transport_info(x , y-2 , stuff_ID, center_ratio );//far down
            far_down = true;
        }
    }    
    
    //calculate the not weighted average filling
    n = 1;    
    ratio = center_ratio;    
    if (left_ratio != -1)
    { 
        ratio += left_ratio;            
        n++;
    }        
    if (right_ratio != -1)        
    {
        ratio += right_ratio;            
        n++;
    }        
    if (down_ratio != -1)
    {
        ratio += down_ratio;            
        n++;
    }
    if (up_ratio != -1)
    {
        ratio += up_ratio;            
        n++;
    }
    ratio /= n;
    // now distribute the stuff
    if (left_ratio != -1)
        { 
            if (!far_left)
            {          
                xx = x - 1;
            }
            else
            {
                xx = x - 2;    
            }
            traffic = equilibrate_transport_stuff(xx, y, &center_lvl, center_cap, ratio, stuff_ID);//left
            if (anim_counter == 0 && traffic > 0 
            && !(world(xx,y)->reportingConstruction->flags & FLAG_POWER_LINE)
            && center_ratio < left_ratio)
            {
                //There is more power on a non powerline and an actual traffic => power source
                anim_counter = POWER_MODULUS;
            }
            if (flashing && world(xx,y)->reportingConstruction->flags & FLAG_POWER_LINE)
            {   //pass on the powerflash downstream
                ConstructionManager::submitRequest
                (
                    new PowerLineFlashRequest(world(xx,y)->reportingConstruction)
                );
            }
        }        
        if (right_ratio != -1)        
        {
            if (!far_right)
            {            
                xx = x + 1; 
            }
            else
            {
                xx = x + 2;    
            }
            traffic = equilibrate_transport_stuff(xx, y, &center_lvl, center_cap, ratio, stuff_ID);//right
            if (anim_counter == 0 && traffic > 0 
            && !(world(xx,y)->reportingConstruction->flags & FLAG_POWER_LINE)
            && center_ratio < right_ratio)
            {
                //There is more power and an actual traffic => power source
                anim_counter = POWER_MODULUS;
            }
            if (flashing && world(xx,y)->reportingConstruction->flags & FLAG_POWER_LINE)
            {   //pass on the powerflash downstream
                ConstructionManager::submitRequest
                (
                    new PowerLineFlashRequest(world(xx,y)->reportingConstruction)
                );
            }
        }
         if (up_ratio != -1)
        {
            if (!far_up)            
            {
                yy = y + 1;
            }
            else
            {
                yy = y + 2;
            }
            traffic = equilibrate_transport_stuff(x, yy, &center_lvl, center_cap, ratio, stuff_ID);//up
            if (anim_counter == 0 && traffic > 0 
            && !(world(x,yy)->reportingConstruction->flags & FLAG_POWER_LINE)
            && center_ratio < up_ratio)
            {
                //There is more power and an actual traffic => power source
                anim_counter = POWER_MODULUS;
            }
            if (flashing && world(x,yy)->reportingConstruction->flags & FLAG_POWER_LINE)
            {   //pass on the powerflash downstream
                ConstructionManager::submitRequest
                (
                    new PowerLineFlashRequest(world(x,yy)->reportingConstruction)
                );
            }
        }         
        if (down_ratio != -1)
        {
            if (!far_down)
            {            
                yy = y - 1;
            }
            else
            {
                yy = y - 2;
            }
            traffic = equilibrate_transport_stuff(x, yy, &center_lvl, center_cap, ratio, stuff_ID);//down
            if (anim_counter == 0 && traffic > 0 
            && !(world(x,yy)->reportingConstruction->flags & FLAG_POWER_LINE)
            && center_ratio < down_ratio)
            {
                //There is more power and an actual traffic => power source
                anim_counter = POWER_MODULUS;
            }
            if (flashing && world(x,yy)->reportingConstruction->flags & FLAG_POWER_LINE)
            {   //pass on the powerflash downstream
                ConstructionManager::submitRequest
                (
                    new PowerLineFlashRequest(world(x,yy)->reportingConstruction)
                );
            }
        }
       
        if (center_lvl < 0)
            std::cout<<"Power < 0 error at "<<constructionGroup->name<<" x,y = "<<x<<","<<y<<std::endl;        
        commodityCount[stuff_ID] = center_lvl;
}

void Powerline::update()
{
    if (commodityCount[STUFF_MWH] > 0)
    {    
        commodityCount[STUFF_MWH]--;// loss on powerline                
        if (anim_counter > 0)
        {                   
            switch (anim_counter)
            {
                case POWER_MODULUS - 2:
                    if (!(type <= 11 && type >= 1))
                        break;
                    flashing = false;
                    type += 11;
                    break;
                case POWER_MODULUS:
                    if (!(type >= 11 && type <= 22))
                        break;
                    flashing = true;                                        
                    type -= 11;
                    break;
            } //end switch anim_counter             
            anim_counter--;
        }
    } // endif MWH
    flow_power();   
}

void Powerline::report()
{
    int i = 0;

    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    list_commodities(&i);
    i++;
    list_connections(&i);
}


/** @file lincity/modules/power_line.cpp */

