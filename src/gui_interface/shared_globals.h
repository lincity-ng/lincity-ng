#ifndef __shared_globals_h__
#define __shared_globals_h__

extern int selected_module_cost;

// main screen
extern int main_screen_originx, main_screen_originy;

extern int monthgraph_size;
extern int *monthgraph_pop;
extern int *monthgraph_starve;
extern int *monthgraph_nojobs;
extern int *monthgraph_ppool;

extern int modern_windmill_flag;

extern int cheat_flag;


// modules
#define NUMOF_MODULES 32
/*
extern int module_help_flag[NUMOF_MODULES];
extern int selected_module;
//extern int selected_module_type; // must be set somehow
extern int sbut[NUMOF_MODULES];

void highlight_module_button (int module);
void unhighlight_module_button (int button);
void set_selected_module (int type);
*/
void update_avail_modules (int popup);


#endif

/** @file gui_interface/shared_globals.h */

