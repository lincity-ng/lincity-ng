#ifndef USER_OPERATION_H__
#define USER_OPERATION_H__

#include "lintypes.h"

class ConstructionGroup;

class UserOperation {
public:
    UserOperation();
    ~UserOperation();
    ConstructionGroup *constructionGroup; // !0 in case of ACTION_BUILD
    unsigned short type;// maybe usefull for showing type as cursor
    int x, y;// where the action should take place
    int selected_module_type; //ButtonID for compatibility

    enum Action //What User wants to do
    {
        ACTION_QUERY,
        ACTION_QUERY_GROUND,
        ACTION_EVACUATE,
        ACTION_BULLDOZE,
        ACTION_FLOOD,
        ACTION_BUILD,
        ACTION_UNKNOWN
    };

    Action action;

private:

};



#endif
