/* ---------------------------------------------------------------------- *
 * src/lincity/modules/all_modules.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "all_modules.hpp"

#include <map>                    // for map
#include <string>                 // for basic_string, operator<

#include "lincity/lintypes.hpp"     // for ConstructionGroup
#include "lincity/resources.hpp"  // for ResourceGroup

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
    ConstructionGroup::addConstructionGroup(&windpowerConstructionGroup);
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
    ConstructionGroup::addConstructionGroup(&parkpondConstructionGroup);
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


    ConstructionGroup::clearResourcepMap();
    ConstructionGroup::addResourceID("Water", &waterConstructionGroup);
    ConstructionGroup::addResourceID("Green", &bareConstructionGroup);
    ConstructionGroup::addResourceID("Tree", &treeConstructionGroup);
    ConstructionGroup::addResourceID("Tree2", &tree2ConstructionGroup);
    ConstructionGroup::addResourceID("Tree3", &tree3ConstructionGroup);
    ConstructionGroup::addResourceID("Desert", &desertConstructionGroup);

    ConstructionGroup::addResourceID("Blacksmith", &blacksmithConstructionGroup);
    ConstructionGroup::addResourceID("FireStation", &fireStationConstructionGroup);
    ConstructionGroup::addResourceID("Health", &healthCentreConstructionGroup);
    ConstructionGroup::addResourceID("SportsCroud", &cricketConstructionGroup);
    ConstructionGroup::addResourceID("Pottery", &potteryConstructionGroup);
    //ConstructionGroup::addResourceID("Monument", &monumentFinishedConstructionGroup);
    ConstructionGroup::addResourceID("MonumentConstruction", &monumentConstructionGroup);
    new ResourceGroup("Monument");
    ConstructionGroup::addResourceID("Waterwell", &waterwellConstructionGroup);
    ConstructionGroup::addResourceID("Commune", &communeConstructionGroup);
    ConstructionGroup::addResourceID("WindMill", &windmillConstructionGroup);
    ConstructionGroup::addResourceID("WindMillHTech", &windpowerConstructionGroup);
    new ResourceGroup("WindMillHTechRG");
    new ResourceGroup("WindMillHTechG");
//  ConstructionGroup::addResourceID("WindMillHTechRG", &windpower_RG_ConstructionGroup);
//  ConstructionGroup::addResourceID("WindMillHTechG", &windpower_G_ConstructionGroup);
    ConstructionGroup::addResourceID("PowerCoalEmpty", &coal_powerConstructionGroup);
    new ResourceGroup("PowerCoalLow");
    new ResourceGroup("PowerCoalMed");
    new ResourceGroup("PowerCoalFull");
    //ConstructionGroup::addResourceID("PowerCoalLow", &coal_power_low_ConstructionGroup);
    //ConstructionGroup::addResourceID("PowerCoalMed", &coal_power_med_ConstructionGroup);
    //ConstructionGroup::addResourceID("PowerCoalFull", &coal_power_full_ConstructionGroup);
    ConstructionGroup::addResourceID("PowerSolar", &solarPowerConstructionGroup);
    new ResourceGroup("Substation");
    ConstructionGroup::addResourceID("SubstationOff", &substationConstructionGroup);
    //ConstructionGroup::addResourceID("Substation", &substation_RG_ConstructionGroup);
    new ResourceGroup("SubstationOn");
    //ConstructionGroup::addResourceID("SubstationOn", &substation_G_ConstructionGroup);
    ConstructionGroup::addResourceID("Mill", &millConstructionGroup);
    ConstructionGroup::addResourceID("Farm", &organic_farmConstructionGroup);
    ConstructionGroup::addResourceID("ResidentialLowLow", &residenceLLConstructionGroup);
    ConstructionGroup::addResourceID("ResidentialMedLow", &residenceMLConstructionGroup);
    ConstructionGroup::addResourceID("ResidentialHighLow", &residenceHLConstructionGroup);
    ConstructionGroup::addResourceID("ResidentialLow", &residenceLHConstructionGroup);
    ConstructionGroup::addResourceID("ResidentialMedHigh", &residenceMHConstructionGroup);
    ConstructionGroup::addResourceID("ResidentialHighHigh", &residenceHHConstructionGroup);
    ConstructionGroup::addResourceID("School", &schoolConstructionGroup);
    ConstructionGroup::addResourceID("University", &universityConstructionGroup);
    ConstructionGroup::addResourceID("Track", &trackConstructionGroup);
    ConstructionGroup::addResourceID("Road", &roadConstructionGroup);
    ConstructionGroup::addResourceID("Rail", &railConstructionGroup);
    ConstructionGroup::addResourceID("TrackBridge", &trackbridgeConstructionGroup);
    ConstructionGroup::addResourceID("RoadBridge", &roadbridgeConstructionGroup);
    ConstructionGroup::addResourceID("RailBridge", &railbridgeConstructionGroup);
    ConstructionGroup::addResourceID("PowerLine", &powerlineConstructionGroup);
    ConstructionGroup::addResourceID("ParklandPlane", &parklandConstructionGroup);
    ConstructionGroup::addResourceID("ParklandLake", &parkpondConstructionGroup);
    ConstructionGroup::addResourceID("OreMine", &oremineConstructionGroup);
    ConstructionGroup::addResourceID("CoalMine", &coalmineConstructionGroup);
    new ResourceGroup("CoalMineLow");
    new ResourceGroup("CoalMineMed");
    new ResourceGroup("CoalMineFull");
    //ConstructionGroup::addResourceID("CoalMineLow", &coalmine_L_ConstructionGroup);
    //ConstructionGroup::addResourceID("CoalMineMed", &coalmine_M_ConstructionGroup);
    //ConstructionGroup::addResourceID("CoalMineFull", &coalmine_H_ConstructionGroup);
    ConstructionGroup::addResourceID("Tip", &tipConstructionGroup);
    ConstructionGroup::addResourceID("Recycle", &recycleConstructionGroup);
    ConstructionGroup::addResourceID("Harbor", &portConstructionGroup);
    ConstructionGroup::addResourceID("Fire", &fireConstructionGroup);
    new ResourceGroup("FireWasteLand");
    //ConstructionGroup::addResourceID("FireWasteLand", &fireWasteLandConstructionGroup);
    ConstructionGroup::addResourceID("IndustryLight", &industryLightConstructionGroup);
    new ResourceGroup("IndustryLightQ");
    new ResourceGroup("IndustryLightL");
    new ResourceGroup("IndustryLightM");
    new ResourceGroup("IndustryLightH");
    //ConstructionGroup::addResourceID("IndustryLightQ", &industryLight_Q_ConstructionGroup);
    //ConstructionGroup::addResourceID("IndustryLightL", &industryLight_L_ConstructionGroup);
    //ConstructionGroup::addResourceID("IndustryLightM", &industryLight_M_ConstructionGroup);
    //ConstructionGroup::addResourceID("IndustryLightH", &industryLight_H_ConstructionGroup);
    ConstructionGroup::addResourceID("IndustryHigh", &industryHeavyConstructionGroup);
    new ResourceGroup("IndustryHighL");
    new ResourceGroup("IndustryHighM");
    new ResourceGroup("IndustryHighH");
    //ConstructionGroup::addResourceID("IndustryHighL", &industryHeavy_L_ConstructionGroup);
    //ConstructionGroup::addResourceID("IndustryHighM", &industryHeavy_M_ConstructionGroup);
    //ConstructionGroup::addResourceID("IndustryHighH", &industryHeavy_H_ConstructionGroup);
    ConstructionGroup::addResourceID("MarketEmpty", &marketConstructionGroup);
    new ResourceGroup("MarketLow");
    new ResourceGroup("MarketMed");
    new ResourceGroup("MarketFull");
    //ConstructionGroup::addResourceID("MarketLow", &market_low_ConstructionGroup);
    //ConstructionGroup::addResourceID("MarketMed", &market_med_ConstructionGroup);
    //ConstructionGroup::addResourceID("MarketFull", &market_full_ConstructionGroup);
    ConstructionGroup::addResourceID("Rocket", &rocketPadConstructionGroup);
    ConstructionGroup::addResourceID("Shanty", &shantyConstructionGroup);

    //resources for extra sprites
    new ResourceGroup("ChildOnSwing"); //used at School
    new ResourceGroup("BlackSmoke");   //used at Coalpower Station
    new ResourceGroup("GraySmoke");    //used at light Industry

    //resources for vehicles
    new ResourceGroup("Bluecar");
    ResourceGroup::resMap["Bluecar"]->is_vehicle = true;


}
