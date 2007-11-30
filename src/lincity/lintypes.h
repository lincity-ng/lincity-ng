/* ---------------------------------------------------------------------- *
 * lintypes.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lintypes_h__
#define __lintypes_h__

/********** Data structures ***************/

struct GROUP {
    const char *name;           /* name of group */
    unsigned short no_credit;   /* TRUE if need credit to build */
    unsigned short group;       /* This is redundant: it must match
                                   the index into the table */
    unsigned short size;
    int colour;                 /* summary map colour */
    int cost_mul;               /* group cost multiplier */
    int bul_cost;               /* group bulldoze cost */
    int fire_chance;            /* probability of fire */
    int cost;                   /* group cost */
    int tech;                   /* group cost */
};

struct TYPE {
    int group;                  /* What group does this type belong to? */
    char *graphic;              /* Bitmap of the graphic */
};

int get_group_of_type(short selected_type);
void set_map_groups(void);
int get_type_cost(short type);
int get_group_cost(short group);
void get_type_name(short type, char *s);

extern struct GROUP main_groups[NUM_OF_GROUPS];
extern struct TYPE main_types[NUM_OF_TYPES];


struct map_point_info_struct {
    int population;
    int flags;
    unsigned short coal_reserve;
    unsigned short ore_reserve;
    int int_1;
    int int_2;
    int int_3;
    int int_4;
    int int_5;
    int int_6;
    int int_7;
};
typedef struct map_point_info_struct Map_Point_Info;

/* Set these flags to true when they need to be updated on the screen */
struct update_scoreboard_struct {
    int mps;
    int mini;
    int date;
    int money;
    int monthly;
    int yearly_1;
    int yearly_2;
    long int message_area;
};
typedef struct update_scoreboard_struct Update_Scoreboard;


/********************** Functions ********************/
/* FIXME: AL1 1290. This shoudl go elsewhere
 * Split this in 2 parts: 
 *      - one for lincity/      wrt engine functions
 *      - one for lincity-ng/   wrt graphic functions
 *
 * and reduce the number of visible function here. (i'm lost , its too much :)
 */


/*
  main functions
  **************
*/
extern int lincity_main(int, char **);
extern void client_main_loop(void);
extern void do_error(const char *);
extern void do_save_city(void);
extern void remove_scene(char *);
extern void save_city(char *);
extern void do_load_city(void);
extern void load_opening_city(char *);
extern void load_city(char *);
extern void reset_animation_times(void);
extern void check_savedir(void);
extern void make_savedir(void);
extern void draw_save_dir(int);
extern void input_save_filename(char *);
extern void load_start_image(void);
extern void start_image_text(void);
extern void si_scroll_text(void);
extern char si_next_char(FILE *);
extern void get_real_time(void);
extern void debug_writeval(int);
extern int cheat(void);
extern void print_cheat(void);
extern void unprint_cheat(void);
extern void order_select_buttons(void);
extern void lincityrc(void);
extern void check_for_old_save_dir(void);
extern int count_groups(int);
extern int compile_results(void);
extern void print_results(void);
extern void mail_results(void);
extern void window_results(void);
extern void init_path_strings(void);
extern void lc_usleep(unsigned long);
extern void dump_tcore(void);
#ifndef LC_X11
    extern void parse_args(int, char **);
#endif
extern void check_endian(void);
extern void eswap32(int *);
extern void eswap16(unsigned short *);
extern void malloc_failure(void);

/*
  fileutil
  ********
*/
void malloc_failure(void);
//FILE* fopen_read_gzipped (char* fn);
//void fclose_read_gzipped (FILE* fp);
void verify_package(void);

/*
  ldsvgui
  *******
*/
void load_opening_city(char *s);
void check_savedir(void);

/*
  opening
  *******
*/
void load_start_image(void);

/*
  type init functions
  *******************
*/
extern void init_types(void);
extern void init_type_graphics(void);
void init_costs(void);
int get_selected_type_cost(short selected_type);
extern char *load_graphic(char *);

/*
  screen functions
  ****************
*/
#ifdef CS_PROFILE
    extern void FPgl_write(int, int, char *);
    extern void FPgl_getbox(int, int, int, int, void *);
    extern void FPgl_putbox(int, int, int, int, void *);
    extern void FPgl_fillbox(int, int, int, int, int);
    extern void FPgl_hline(int, int, int, int);
    extern void FPgl_line(int, int, int, int, int);
    extern void FPgl_setpixel(int, int, int);
#endif
extern void check_power_line_graphics(void);
extern void check_water_graphics(void);
extern void check_track_graphics(void);
extern void check_rail_graphics(void);
extern void check_road_graphics(void);
extern int select_power_line_type(int, int, int, int);
extern int select_water_type(int, int, int, int);
extern int select_track_type(int, int, int, int);
extern int select_rail_type(int, int, int, int);
extern int select_road_type(int, int, int, int);
extern void update_select_buttons(void);
extern void draw_main_window_box(int);
extern void draw_select_button_graphic(int, char *);
extern void setcustompalette(void);
extern void clip_main_window(void);
extern void unclip_main_window(void);
extern void initfont(void);
extern void load_scroll_buttons(void);
extern void draw_scroll_buttons(void);
extern void update_mini_screen(void);
extern void draw_mini_screen(void);
extern void draw_big_mini_screen(void);
extern void draw_mini_screen_pollution(void);
extern void draw_mini_screen_fire_cover(void);
extern void draw_mini_screen_cricket_cover(void);
extern void draw_mini_screen_health_cover(void);
extern void draw_mini_screen_ub40(void);
extern void draw_mini_screen_starve(void);
extern void draw_mini_screen_coal(void);
extern void draw_mini_screen_power(void);
extern void draw_mini_screen_ocost(void);
extern void draw_mini_screen_port(void);
extern void draw_mini_screen_cursor(void);
extern void print_stats(void);
extern void print_total_money(void);
int ask_launch_rocket_now(int x, int y);
void print_date(void);
extern void draw_market_cb(void);
extern void clicked_market_cb(int, int);
extern void close_market_cb(void);
extern void draw_port_cb(void);
extern void clicked_port_cb(int, int);
extern void close_port_cb(void);
extern int yn_dial_box(const char *, const char *, const char *, const char *);
extern void ok_dial_box(const char *, int, const char *);
extern int inv_sbut(int);
extern void call_select_change_up(int);
extern void beg_space_pad(const char *, int);
extern void draw_up_pbar(int, int, int, int);
extern void draw_down_pbar(int, int, int, int);
extern void do_pbar_population(int);
extern void do_pbar_tech(int);
extern void do_pbar_food(int);
extern void do_pbar_jobs(int);
extern void do_pbar_coal(int);
extern void do_pbar_goods(int);
extern void do_pbar_ore(int);
extern void do_pbar_steel(int);
extern void do_pbar_money(int);
extern void prog_box(const char *, int);
/*
  mouse functions -- are now in mouse.h, where they should be!
  ***************
*/

/*
  engine functions
  ****************
*/
void initialize_tax_rates(void);
void set_mappoint(int x, int y, short selected_type);
void set_mappoint_used(int, int, int, int);
void set_mappoint_ints(int fromx, int fromy, int x, int y);
void new_city(int *originx, int *originy, int random_village);
//extern void engine_do_time_step(void);
extern void do_residence(int, int);
extern void debug_print(int);
extern void do_power_source(int, int);
extern void do_power_source_coal(int, int);
extern void do_industry_l(int, int);
extern void do_industry_h(int, int);
extern void do_power_substation(int, int);
extern int get_power(int, int, int, int);
extern int add_a_substation(int, int);
extern void remove_a_substation(int, int);
extern void do_organic_farm(int, int);
extern void shuffle_substations(void);
extern void do_coalmine(int, int);
extern void do_oremine(int, int);
extern void do_commune(int, int);
extern void do_port(int, int);

extern void do_parkland(int, int);
extern void do_university(int, int);
extern void do_recycle(int, int);
extern void do_health_centre(int, int);
extern void do_rocket_pad(int, int);
extern void launch_rocket(int x, int y);
extern void do_windmill(int, int);
extern void do_monument(int, int);
extern void do_school(int, int);
extern void do_blacksmith(int, int);
extern void do_mill(int, int);
extern void do_pottery(int, int);
extern void do_firestation(int, int);
extern void do_cricket(int, int);
extern void do_fire_cover(int, int);
extern void do_health_cover(int, int);
extern void do_cricket_cover(int, int);
extern void do_random_fire(int, int, int);
extern void do_fire(int, int);
extern void add_a_shanty(void);
extern void remove_a_shanty(int, int);
extern void update_shanty(void);
extern void do_shanty(int, int);
extern void do_tip(int, int);
extern void update_tech_dep(int, int);
extern void do_waterwell_cover(int, int);
/*
   transport functions
   *******************
*/
void connect_transport(int originx, int originy, int w, int h);
extern void do_power_line(int, int);
extern void do_track(int, int);
extern void do_rail(int, int);
extern void do_road(int, int);
extern void general_transport(Map_Point_Info *, int *, int, int *);

/*
   market functions
   ****************
*/
extern int get_food(int, int, int);
extern int put_food(int, int, int);
extern int get_jobs(int, int, int);
extern int put_jobs(int, int, int);
extern int get_goods(int, int, int);
extern int put_goods(int, int, int);
extern int get_ore(int, int, int);
extern int put_ore(int, int, int);
extern int get_coal(int, int, int);
extern int put_coal(int, int, int);
extern int add_a_market(int, int);
extern void remove_a_market(int, int);
extern void do_market(int, int);
extern void shuffle_markets(void);
extern int deal_with_transport(int, int, int, int);
extern int get_steel(int, int, int);
extern int put_steel(int, int, int);
extern int get_waste(int, int, int);
extern int put_waste(int, int, int);
extern int get_stuff(int, int, int, int);
extern int get_stuff2(Map_Point_Info *, int, int);
extern int get_stuff3(Map_Point_Info *, int, int);
extern int get_stuff4(Map_Point_Info *, int, int);
extern int put_stuff(int, int, int, int);
extern int put_stuff2(Map_Point_Info *, short *, int, int);
extern int put_stuff3(Map_Point_Info *, short *, int, int);
extern int put_stuff4(Map_Point_Info *, short *, int, int);

#if defined SDL
#   warning "SDL defined"
#   include <SDL/SDL.h>

    extern void sdl_gl_write(int, int, char *);
    extern void sdl_gl_getbox(int, int, int, int, void *);
    extern void sdl_gl_putbox(int, int, int, int, void *);
    extern void sdl_gl_fillbox(int, int, int, int, Uint32 col);
    
    extern void sdl_gl_hline(int x1, int y1, int x2, Uint32 col);
    extern void sdl_gl_vline(int x1, int y1, int y2, Uint32 col);
    extern void sdl_gl_line(int x1, int y1, int x2, int y2, Uint32 col);
    
    extern void sdl_gl_setpixel(int x, int y, Uint32 col);
    extern void sdl_gl_setfontcolors(Uint32 bg, Uint32 fg);
    extern void sdl_gl_setfont(int, int, void *);
    extern void sdl_gl_enableclipping(void);
    extern void sdl_gl_setclippingwindow(int, int, int, int);
    extern void sdl_gl_disableclipping(void);
    
    extern void sdl_blit(int x, int y, int w, int h, SDL_Surface *);
    
    /* Wait for a key or some other event */
    extern int sdl_wait_for_key(void);
    extern int sdl_update_wait_for_key(void);
    
    extern int sdl_check_key(void);
    extern int sdl_update_check_key(void);
    
#   define sdl_map_rgb(r,g,b) SDL_MapRGB(display.surface->format, r, g, b)
#   define sdl_video() (display.surface)
#   define sdl_flip() SDL_Flip(display.surface)
    
    extern void sdl_clear(Uint32 col);
    
#   define Fgl_write(a,b,c) sdl_gl_write(a,b,c)
#   define Fgl_getbox(a,b,c,d,e) sdl_gl_getbox(a,b,c,d,e)
#   define Fgl_putbox(a,b,c,d,e) sdl_gl_putbox(a,b,c,d,e)
#   define Fgl_fillbox(a,b,c,d,e) sdl_gl_fillbox(a,b,c,d,e)
#   define Fgl_hline(a,b,c,d) sdl_gl_hline(a,b,c,d)
#   define Fgl_line(a,b,c,d,e) sdl_gl_line(a,b,c,d,e)
#   define Fgl_setpixel(a,b,c) sdl_gl_setpixel(a,b,c)
#   define Fgl_setfontcolors(a,b) sdl_gl_setfontcolors(a,b)
#   define Fgl_setfont(a,b,c) sdl_gl_setfont(a,b,c)
#   define Fgl_enableclipping() sdl_gl_enableclipping()
#   define Fgl_setclippingwindow(a,b,c,d) sdl_gl_setclippingwindow(a,b,c,d)
#   define Fgl_disableclipping() sdl_gl_disableclipping()
   
#elif defined LC_X11
    /* ----- X11 functions ----- */
    extern void Fgl_write(int, int, const char *);
    extern void open_write(int, int, const char *);
    extern void Fgl_getbox(int, int, int, int, void *);
    extern void Fgl_putbox(int, int, int, int, void *);
    extern void Fgl_fillbox(int, int, int, int, int);
    extern void Fgl_hline(int, int, int, int);
    extern void Fgl_line(int, int, int, int, int);
    extern void Fgl_setpixel(int, int, int);
    extern void Fgl_setfontcolors(int, int);
    extern void Fgl_setfont(int, int, void *);
    extern void Fgl_enableclipping(void);
    extern void Fgl_setclippingwindow(int, int, int, int);
    extern void Fgl_disableclipping(void);
#   ifdef USE_X11_PIXMAPS
#       define USE_PIXMAPS
        extern void init_pixmaps(void);
        extern void init_icon_pixmap(short);
        extern void update_pixmap(int, int, int, int, int, int, int, char *);
#   endif
    
#elif defined (WIN32)
    /* ----- Win32 functions ----- */
    extern void gl_setpalettecolor(long, long, long, long);
    extern void Fgl_write(int, int, const char *);
    extern void open_write(int, int, const char *);
    extern void Fgl_getbox(int, int, int, int, void *);
    extern void Fgl_putbox(int, int, int, int, void *);
    extern void Fgl_fillbox(int, int, int, int, int);
    extern void Fgl_hline(int, int, int, int);
    extern void Fgl_line(int, int, int, int, int);
    extern void Fgl_setpixel(int, int, int);
    extern void Fgl_setfontcolors(int, int);
    extern void Fgl_setfont(int, int, void *);
    extern void Fgl_enableclipping(void);
    extern void Fgl_setclippingwindow(int, int, int, int);
    extern void Fgl_disableclipping(void);
#   define USE_PIXMAPS             /* Always */
    extern void init_pixmaps(void);
    extern void init_icon_pixmap(short);
    extern void update_pixmap(int, int, int, int, int, int, int, char *);
    
#else
    /* ----- SVGALIB functions ----- */
    /* profiling */
#   ifdef CS_PROFILE
#       define Fgl_write(a,b,c) FPgl_write(a,b,c)
#       define Fgl_getbox(a,b,c,d,e) FPgl_getbox(a,b,c,d,e)
#       define Fgl_putbox(a,b,c,d,e) FPgl_putbox(a,b,c,d,e)
#       define Fgl_fillbox(a,b,c,d,e) FPgl_fillbox(a,b,c,d,e)
#       define Fgl_hline(a,b,c,d) FPgl_hline(a,b,c,d)
#       define Fgl_line(a,b,c,d,e) FPgl_line(a,b,c,d,e)
#       define Fgl_setpixel(a,b,c) FPgl_setpixel(a,b,c)
#       define Fgl_setfontcolors(a,b) gl_setfontcolors(a,b)
#       define Fgl_setfont(a,b,c) gl_setfont(a,b,c)
#       define Fgl_enableclipping()   gl_enableclipping()
#       define Fgl_setclippingwindow(a,b,c,d)   gl_setclippingwindow(a,b,c,d)
#       define Fgl_disableclipping()   gl_disableclipping()
#   else
#       define Fgl_write(a,b,c) gl_write(a,b,c)
#       define Fgl_getbox(a,b,c,d,e) gl_getbox(a,b,c,d,e)
#       define Fgl_putbox(a,b,c,d,e) gl_putbox(a,b,c,d,e)
#       define Fgl_fillbox(a,b,c,d,e) gl_fillbox(a,b,c,d,e)
#       define Fgl_hline(a,b,c,d) gl_hline(a,b,c,d)
#       define Fgl_line(a,b,c,d,e) gl_line(a,b,c,d,e)
#       define Fgl_setpixel(a,b,c) gl_setpixel(a,b,c)
#       define Fgl_setfontcolors(a,b) gl_setfontcolors(a,b)
#       define Fgl_setfont(a,b,c) gl_setfont(a,b,c)
#       define Fgl_enableclipping()   gl_enableclipping()
#       define Fgl_setclippingwindow(a,b,c,d)   gl_setclippingwindow(a,b,c,d)
#       define Fgl_disableclipping()   gl_disableclipping()
#   endif

#endif
#endif /* __lintypes_h__ */
