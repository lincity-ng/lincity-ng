#include "ConstructionManager.h"
#include "lintypes.h"
//FIXME
#include <iostream>

void ConstructionManager::submitRequest(ConstructionRequest *request) {
    std::map<Construction *, ConstructionRequest *>::iterator iterator;
    iterator = pendingRequests.find(request->subject);
    if (iterator != pendingRequests.end())
    {
        //std::cout<<"duplicate Request at "<<request->subject->constructionGroup->name<<" x,y = "<<request->subject->x<<","<<request->subject->y<<std::endl;        
        delete iterator->second;
        iterator->second = request;
    }
    else
    {
        //std::cout<<"added Request at "<<request->subject->constructionGroup->name<<" x,y = "<<request->subject->x<<","<<request->subject->y<<std::endl;        
        pendingRequests[request->subject] = request;
    }
}

void ConstructionManager::executeRequest(ConstructionRequest *request) {
    request->execute();
    delete request;
}

void ConstructionManager::executePendingRequests() {
    std::map<Construction *, ConstructionRequest *>::const_iterator iterator;
    for (iterator = pendingRequests.begin(); iterator != pendingRequests.end(); iterator++)
    {
        executeRequest(iterator->second);
    }
    pendingRequests.clear();
}

std::map<Construction *, ConstructionRequest *> ConstructionManager::pendingRequests;

