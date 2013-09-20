#include "UserOperation.h"
#include <iostream>

UserOperation::UserOperation()
{
    x = y = 0;
    constructionGroup = NULL;
    type = 0;
    action = ACTION_QUERY;
}

UserOperation::~UserOperation() {}


