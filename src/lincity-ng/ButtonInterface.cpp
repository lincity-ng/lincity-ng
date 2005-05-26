#include <config.h>

#include "gui_interface/shared_globals.h"

int module_help_flag[NUMOF_MODULES]; 
int selected_module;	
int sbut[NUMOF_MODULES];

void highlight_module_button (int module)
{
    (void) module;
}

void unhighlight_module_button (int button)
{
    (void) button;
}

void set_selected_module (int type)
{
    (void) type;
}

