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

#endif /* __lintypes_h__ */
