#ifndef __module_buttons_h__
#define __module_buttons_h__

void init_module_buttons (void);

// screen.c: void draw_select_buttons (void);
void draw_module_buttons (void);

// geometry.c: void draw_select_button_graphic (int button, char *graphic)
void draw_module (int button, char * graphic);

// screen.c: void call_select_change_up (int button)
void activate_module (int module);

// mouse.c: void do_mouse_select_buttons (int rawx, int rawy, int mbutton)
void handle_module_buttons (int x, int y, int mbutton);

// mouse.c: void do_select_button (int button, int mbutton);
void select_module (int module, int mbutton);

// mouse.c: void highlight_select_button (int button);
void highlight_module_button (int module);

// mouse.c: void unhighlight_select_button (int button);
void unhighlight_module_button (int button);

// mouse.c: draw_module_cost (int grp);
void draw_selected_module_cost (void);

// screen.c: void draw_select_buttons (void);
void draw_modules (void);

// screen.c: void update_select_buttons (void);
void update_avail_modules (void);

// mouse.c: int inv_sbut (int button);
int inv_sbut (int module);


#define NUMOF_MODULES 32
// #define NUMOF_SELECT_BUTTONS_DOWN 16

extern int sbut[NUMOF_MODULES];

extern int module_type[NUMOF_MODULES];

extern int selected_module;	
extern int old_selected_module;
extern int selected_module_group;
extern int selected_module_type;
extern int selected_module_cost;

/*extern char *module_graphic[NUMOF_MODULES];
extern int module_tflag[NUMOF_MODULES];
extern char module_help[NUMOF_MODULES][20]; */
extern int module_help_flag[NUMOF_MODULES]; 






#endif
