#include "UserOperation.h"
#include <iostream>
#include "lin-city.h"

static int last_warning_message_group = 0;

UserOperation::UserOperation()
{
    constructionGroup = NULL;
    type = 0;
    action = ACTION_QUERY;
}

UserOperation::~UserOperation() {}

extern void ok_dial_box(const char *, int, const char *);

bool
UserOperation::is_allowed_here(int x, int y)
{
    switch(action)
    {
        case ACTION_QUERY:
            return world.is_visible(x,y);
        case ACTION_BUILD:
        {
            if(!(world.is_visible(x,y) &&
            world.is_visible(x + constructionGroup->size - 1, y + constructionGroup->size - 1)))
            {   return false;}
             //At last check for bare building site
            for(int j = 0; j<constructionGroup->size; j++)
            {
                for(int i = 0; i<constructionGroup->size; i++)
                {
                    if(!world(x+i, y+j)->is_bare())
                    {   return false;}
                }
            }
            bool msg = (last_warning_message_group != constructionGroup->group);
            msg = constructionGroup->is_allowed_here(x, y, msg);
            if(!msg)
            {   last_warning_message_group = constructionGroup->group;}
            else
            {   last_warning_message_group = 0;}
            return msg;
        }
        case ACTION_BULLDOZE:
        //The cursor never tells about problems with bulldozer
        //check_bulldoze_area in MapEdit is careful anyways
            return world.is_visible(x,y);
        case ACTION_EVACUATE:
            return (world.is_visible(x,y) && world(x,y)->reportingConstruction &&
            ! (world(x,y)->reportingConstruction->flags & FLAG_NEVER_EVACUATE));
        case ACTION_FLOOD:
            return (world.is_visible(x,y) && world(x,y)->is_bare());
        default:
            std::cout << "default in UserOperation.action" << std::endl;
            return false;
    }
    return false;
}
