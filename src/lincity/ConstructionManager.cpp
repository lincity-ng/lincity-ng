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
    std::map<Construction *, ConstructionRequest *>::iterator iterator;
    iterator = pendingRequests.find(request->subject);
    if (iterator != pendingRequests.end())
    {
        //make sure there wont be no another pending request after execution 
        if (request != iterator->second)
        {
			delete iterator->second;
			pendingRequests.erase(iterator->first);
		}
    }
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

void ConstructionManager::clearRequests()
{
	pendingRequests.clear();
}


std::map<Construction *, ConstructionRequest *> ConstructionManager::pendingRequests;

