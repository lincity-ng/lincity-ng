
//#include "ag_debug.h"
#include "lincity/engglobs.h"
#include "gui_interface/pbar_interface.h"
#include "PBar.hpp"
#include "lincity/stats.h"

void pbars_full_refresh (void)
{
  //  TRACE;
}

void
update_pbar (int pbar_num, int value, int month_flag)
{

  //  TRACE;
  //  cdebug(pbar_num<<":"<<value<<":"<<month_flag);
  if(LCPBarInstance)
    LCPBarInstance->setValue(pbar_num,value);
}

void 
refresh_pbars (void)
{
  //  TRACE;
}
void
init_pbars (void)
{
  //  TRACE;
}

struct pbar_st pbars[NUM_PBARS];

void
update_pbars_monthly()
{
    update_pbar (PPOP, housed_population + people_pool, 1);
    update_pbar (PTECH, tech_level, 1);
    update_pbar (PFOOD, tfood_in_markets / data_last_month, 1);
    update_pbar (PJOBS, tjobs_in_markets / data_last_month, 1);
    update_pbar (PCOAL, tcoal_in_markets / data_last_month, 1);
    update_pbar (PGOODS, tgoods_in_markets / data_last_month, 1);
    update_pbar (PORE, tore_in_markets / data_last_month, 1);
    update_pbar (PSTEEL, tsteel_in_markets / data_last_month, 1);
    update_pbar (PMONEY, total_money, 1);
}
