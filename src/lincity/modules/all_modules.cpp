#include "all_modules.h"

void initializeModules() {
    ConstructionGroup::clearGroupMap();
    ConstructionGroup::addConstructionGroup(&fireStationConstructionGroup);
    ConstructionGroup::addConstructionGroup(&healthCentreConstructionGroup);
    ConstructionGroup::addConstructionGroup(&cricketConstructionGroup);
    ConstructionGroup::addConstructionGroup(&potteryConstructionGroup);
    ConstructionGroup::addConstructionGroup(&monumentConstructionGroup);
    ConstructionGroup::addConstructionGroup(&blacksmithConstructionGroup);
    ConstructionGroup::addConstructionGroup(&waterwellConstructionGroup);
    ConstructionGroup::addConstructionGroup(&communeConstructionGroup);
    ConstructionGroup::addConstructionGroup(&windmillConstructionGroup);
    ConstructionGroup::addConstructionGroup(&coal_powerConstructionGroup);
    ConstructionGroup::addConstructionGroup(&solarPowerConstructionGroup);
    ConstructionGroup::addConstructionGroup(&substationConstructionGroup);
    ConstructionGroup::addConstructionGroup(&millConstructionGroup);
    ConstructionGroup::addConstructionGroup(&organic_farmConstructionGroup);
    ConstructionGroup::addConstructionGroup(&residenceLLConstructionGroup);
    ConstructionGroup::addConstructionGroup(&residenceMLConstructionGroup);
    ConstructionGroup::addConstructionGroup(&residenceHLConstructionGroup);
    ConstructionGroup::addConstructionGroup(&residenceLHConstructionGroup);
    ConstructionGroup::addConstructionGroup(&residenceMHConstructionGroup);
    ConstructionGroup::addConstructionGroup(&residenceHHConstructionGroup);
    ConstructionGroup::addConstructionGroup(&schoolConstructionGroup);
    ConstructionGroup::addConstructionGroup(&universityConstructionGroup);
    ConstructionGroup::addConstructionGroup(&trackConstructionGroup);
    ConstructionGroup::addConstructionGroup(&roadConstructionGroup);
    ConstructionGroup::addConstructionGroup(&railConstructionGroup);
    ConstructionGroup::addConstructionGroup(&trackbridgeConstructionGroup);
    ConstructionGroup::addConstructionGroup(&roadbridgeConstructionGroup);
    ConstructionGroup::addConstructionGroup(&railbridgeConstructionGroup);
    ConstructionGroup::addConstructionGroup(&powerlineConstructionGroup);
    ConstructionGroup::addConstructionGroup(&parklandConstructionGroup);
    ConstructionGroup::addConstructionGroup(&oremineConstructionGroup);
    ConstructionGroup::addConstructionGroup(&coalmineConstructionGroup);
    ConstructionGroup::addConstructionGroup(&tipConstructionGroup);
    ConstructionGroup::addConstructionGroup(&recycleConstructionGroup);
    ConstructionGroup::addConstructionGroup(&portConstructionGroup);
    ConstructionGroup::addConstructionGroup(&fireConstructionGroup);
    ConstructionGroup::addConstructionGroup(&industryLightConstructionGroup);
    ConstructionGroup::addConstructionGroup(&industryHeavyConstructionGroup);
    ConstructionGroup::addConstructionGroup(&marketConstructionGroup);
    ConstructionGroup::addConstructionGroup(&rocketPadConstructionGroup);
    ConstructionGroup::addConstructionGroup(&shantyConstructionGroup);    
}

