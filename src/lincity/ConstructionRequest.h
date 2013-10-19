#ifndef CONSTRUCTION_REQUEST_H__
#define CONSTRUCTION_REQUEST_H__

class Construction;
class ConstructionManager;

class ConstructionRequest {
public:
    ConstructionRequest(Construction *subject): subject(subject) { }
    virtual ~ConstructionRequest() { }
    virtual void execute() = 0;
protected:
    Construction *subject;
    friend class ConstructionManager;
};

class ConstructionDeletionRequest: public ConstructionRequest {
public:
    ConstructionDeletionRequest(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};

class OreMineDeletionRequest: public ConstructionRequest {
public:
    OreMineDeletionRequest(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};

class CommuneDeletionRequest: public ConstructionRequest {
public:
    CommuneDeletionRequest(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};

class BurnDownRequest: public ConstructionRequest {
public:
    BurnDownRequest(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};

class SetOnFire: public ConstructionRequest {
public:
    SetOnFire(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};


class PowerLineFlashRequest: public ConstructionRequest {
public:
    PowerLineFlashRequest(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};


#endif

