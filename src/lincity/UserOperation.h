#ifndef USER_OPERATION_H__
#define USER_OPERATION_H__

#include "lintypes.h"

class ConstructionGroup;

class UserOperation {
public:
    UserOperation();
    ~UserOperation();
    ConstructionGroup *constructionGroup; // !0 in case of ACTION_BUILD
    //unsigned short type;// maybe usefull for showing type as cursor
    int selected_module_type; //ButtonID for ButtonPanel


    enum Action //What User wants to do
    {
        ACTION_QUERY,
        ACTION_EVACUATE,
        ACTION_BULLDOZE,
        ACTION_FLOOD,
        ACTION_BUILD,
        ACTION_UNKNOWN
    };

    Action action;
    bool is_allowed_here(int x, int y, bool warning);
    bool enoughTech();// tells if scaled tech_level is sufficient
    float requiredTech(); // returns scaled tech_level for tooltipp info
    std::string createTooltip( /*int module,*/ bool root = true );
private:
};



#endif
