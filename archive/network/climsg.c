/* ---------------------------------------------------------------------- *
 * climsg.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "lcstring.h"
#include "lin-city.h"
#include "lctypes.h"
#include "engglobs.h"
#include "cliglobs.h"
#include "clinet.h"
#include "shrnet.h"
#include "screen.h"

/* ---------------------------------------------------------------------- *
 * External Global Variables
 * ---------------------------------------------------------------------- */
extern Update_Scoreboard update_scoreboard;
extern MsgBuf recvbuf;

extern int tfood_in_markets;

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
void handle_msg_unimplemented (void);
void handle_msg_map_types (void);
void handle_msg_map_zoom (void);
void handle_msg_item_placed (void);
void handle_msg_mps_data (void);
void handle_msg_monthly (void);
void handle_msg_yearly (void);
void handle_msg_map_types_region (void);
void handle_msg_main_scrn_data (void);
void handle_msg_flags_changed (void);
void handle_msg_mini_data (void);
void handle_msg_coal_survey_done (void);
void handle_msg_rocket_built (void);
void handle_msg_rocket_fired (void);


/* ---------------------------------------------------------------------- *
 * Private Global Variables
 * ---------------------------------------------------------------------- */
void (*msg_handlers[NUM_CLIENT_MSG_TYPES])(void) = 
{
    handle_msg_map_types,
    handle_msg_map_zoom,
    handle_msg_item_placed,
    handle_msg_mps_data,
    handle_msg_monthly,
    handle_msg_yearly,
    handle_msg_map_types_region,
    handle_msg_main_scrn_data,
    handle_msg_flags_changed,
    handle_msg_mini_data,
    handle_msg_coal_survey_done,
    handle_msg_rocket_built,
    handle_msg_rocket_fired
};


/* ---------------------------------------------------------------------- *
 * Public Functions -- Incoming messages
 * ---------------------------------------------------------------------- */
void 
client_handle_message (void)
{
    MsgBuf* msg = &recvbuf;
    if (MSG_OPCODE(msg) >= SERVER_MSG_MIN && 
	MSG_OPCODE(msg) <= SERVER_MSG_MAX) {
	printf ("Client -- handling message..\n");
	msg_handlers[MSG_OPCODE(msg)-SERVER_MSG_MIN]();
    } else {
	printf ("Warning. Client received an ill-formed message (type=%d)\n",
		MSG_OPCODE(msg));
    }
}


/* ---------------------------------------------------------------------- *
 * Public Functions -- Outgoing messages
 * ---------------------------------------------------------------------- */
void 
network_join_game (void)
{
  MsgBuf msg;
  MSG_OPCODE(&msg) = MSG_JOIN;
  MSG_LENGTH(&msg) = 0;
  net_send_message (sock, &msg);
}

void 
network_new_city (int* originx, int* originy, int random_village)
{
  MsgBuf msg;
  MSG_OPCODE(&msg) = MSG_NEW_CITY;
  MSG_LENGTH(&msg) = 0;
  net_send_message (sock, &msg);
}


void
network_place_item (int x, int y, int selected_type)
{
  MsgBuf msg;
  Int32* msgp = (Int32*) MSG_BODY(&msg);

  MSG_OPCODE(&msg) = MSG_PLACE_ITEM;
  MSG_LENGTH(&msg) = 3 * sizeof(Int32);
  *msgp++ = htonl(x);
  *msgp++ = htonl(y);
  *msgp++ = htonl(selected_type);
  net_send_message (sock, &msg);
}

void
network_bulldoze_item (int x, int y)
{
  MsgBuf msg;
  Int32* msgp = (Int32*) MSG_BODY(&msg);

  MSG_OPCODE(&msg) = MSG_BULLDOZE_ITEM;
  MSG_LENGTH(&msg) = 2 * sizeof(Int32);
  *msgp++ = htonl(x);
  *msgp++ = htonl(y);
  net_send_message (sock, &msg);
}

void
network_request_mappoint_stats (int x, int y)
{
  MsgBuf msg;
  Int32* msgp = (Int32*) MSG_BODY(&msg);
  MSG_OPCODE(&msg) = MSG_MPS_REQ;
  MSG_LENGTH(&msg) = 2 * sizeof(Int32);
  *msgp++ = htonl(x);
  *msgp++ = htonl(y);
  net_send_message (sock, &msg);
}

void
network_unrequest_mappoint_stats (void)
{
    MsgBuf msg;
    MSG_OPCODE(&msg) = MSG_MPS_END;
    MSG_LENGTH(&msg) = 0;
    net_send_message (sock, &msg);
}

void
network_request_main_screen (void)
{
  MsgBuf msg;
  Int32* msgp = (Int32*) MSG_BODY(&msg);
  MSG_OPCODE(&msg) = MSG_MAIN_SCRN_REQ;
  MSG_LENGTH(&msg) = 4 * sizeof(Int32);
  *msgp++ = htonl(main_screen_originx);
  *msgp++ = htonl(main_screen_originy);
#if defined (commentout)
  /* GCS FIX: Need to give server right size of main screen */
  *msgp++ = htonl(MAIN_WIN_W / 16);
  *msgp++ = htonl(MAIN_WIN_H / 16);
#endif
  net_send_message (sock, &msg);
}

void
network_unrequest_main_screen (void)
{
    MsgBuf msg;
    MSG_OPCODE(&msg) = MSG_MAIN_SCRN_END;
    MSG_LENGTH(&msg) = 0;
    net_send_message (sock, &msg);
}

void
network_send_flags (int x, int y)
{
    MsgBuf msg;
    Int32* msgp = (Int32*) MSG_BODY(&msg);
    MSG_OPCODE(&msg) = MSG_SEND_FLAGS;
    MSG_LENGTH(&msg) = 3 * sizeof(Int32);
    *msgp++ = htonl(x);
    *msgp++ = htonl(y);
    *msgp++ = htonl(MP_INFO(x,y).flags);
    net_send_message (sock, &msg);
}

void
network_request_mini_screen (int mini_type)
{
    MsgBuf msg;
    Int32* msgp = (Int32*) MSG_BODY(&msg);
    MSG_OPCODE(&msg) = MSG_MINI_SCRN_REQ;
    MSG_LENGTH(&msg) = 1 * sizeof(Int32);
    *msgp++ = htonl(mini_type);
    net_send_message (sock, &msg);
}

void
network_unrequest_mini_screen (void)
{
    MsgBuf msg;
    MSG_OPCODE(&msg) = MSG_MINI_SCRN_END;
    MSG_LENGTH(&msg) = 0;
    net_send_message (sock, &msg);
}

void 
network_do_coal_survey (void)
{
    MsgBuf msg;
    MSG_OPCODE(&msg) = MSG_DO_COAL_SURVEY;
    MSG_LENGTH(&msg) = 0;
    net_send_message (sock, &msg);
}

void 
network_launch_rocket (int x, int y)
{
    MsgBuf msg;
    Int32* msgp = (Int32*) MSG_BODY(&msg);
    MSG_OPCODE(&msg) = MSG_LAUNCH_ROCKET;
    MSG_LENGTH(&msg) = 2 * sizeof(Int32);
    *msgp++ = htonl(x);
    *msgp++ = htonl(y);
    net_send_message (sock, &msg);
}

/* ---------------------------------------------------------------------- *
 * Private Functions
 * ---------------------------------------------------------------------- */
void 
handle_msg_unimplemented (void)
{
    MsgBuf* msg = &recvbuf;
    printf ("Warning. Client received unimplemented message type (type=%d)\n",
	    MSG_OPCODE(msg));
}

void 
handle_msg_map_types (void)
{
    int x, y;
    MsgBuf* msg = &recvbuf;
    Int16* msgp = (Int16*) MSG_BODY(msg);

    printf ("Client received MSG_MAP_TYPES.\n");
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    MP_TYPE(x,y) = ntohs(*msgp++);
	    if (MP_TYPE(x,y) != CST_USED && MP_TYPE(x,y) != CST_GREEN) {
		set_mappoint (x, y, MP_TYPE(x,y));
	    }
	}
    }
}

void 
handle_msg_map_zoom (void)
{
  MsgBuf* msg = &recvbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  printf ("Client received MSG_MAP_ZOOM.\n");
  main_screen_originx = ntohl(*msgp++);
  main_screen_originy = ntohl(*msgp++);

  network_request_main_screen ();
  refresh_main_screen ();
}

void 
handle_msg_item_placed (void)
{
  int x, y;
  unsigned short selected_item;
  MsgBuf* msg = &recvbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  printf ("Client received MSG_ITEM_PLACED.\n");
  x = ntohl(*msgp++);
  y = ntohl(*msgp++);
  selected_item = (unsigned short) ntohl(*msgp++);

  set_mappoint (x, y, selected_item);
  refresh_main_screen ();
}

void 
handle_msg_mps_data (void)
{
  int x, y;
  MsgBuf* msg = &recvbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  printf ("Client received MSG_MPS_DATA.\n");
  x = ntohl(*msgp++);
  y = ntohl(*msgp++);
  MP_INFO(x,y).population = ntohl(*msgp++);
  MP_INFO(x,y).flags = ntohl(*msgp++);
  MP_INFO(x,y).int_1 = ntohl(*msgp++);
  MP_INFO(x,y).int_2 = ntohl(*msgp++);
  MP_INFO(x,y).int_3 = ntohl(*msgp++);
  MP_INFO(x,y).int_4 = ntohl(*msgp++);
  MP_INFO(x,y).int_5 = ntohl(*msgp++);
  MP_INFO(x,y).int_6 = ntohl(*msgp++);
  MP_INFO(x,y).int_7 = ntohl(*msgp++);
  MP_POL(x,y) = ntohl(*msgp++);

  update_scoreboard.mps = 1;
}

void 
handle_msg_monthly (void)
{
  MsgBuf* msg = &recvbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  printf ("Client received MSG_MONTHLY.\n");
  food_in_markets = ntohl(*msgp++);
  housed_population = ntohl(*msgp++);
  numof_shanties = ntohl(*msgp++);
  people_pool = ntohl(*msgp++);
  population = ntohl(*msgp++);
  rockets_launched = ntohl(*msgp++);
  rockets_launched_success = ntohl(*msgp++);
  tech_level = ntohl(*msgp++);
  tfood_in_markets = ntohl(*msgp++);
  total_time = ntohl(*msgp++);
  tpopulation = ntohl(*msgp++);
  tstarving_population = ntohl(*msgp++);
  tunemployed_population = ntohl(*msgp++);
  unnat_deaths = ntohl(*msgp++);

  update_scoreboard.monthly = 1;
  update_scoreboard.date = 1;
}

void 
handle_msg_yearly (void)
{
  MsgBuf* msg = &recvbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  printf ("Client received MSG_YEARLY.\n");

  total_money = ntohl(*msgp++);
  income_tax = ntohl(*msgp++);
  coal_tax = ntohl(*msgp++);
  goods_tax = ntohl(*msgp++);
  export_tax = ntohl(*msgp++);
  other_cost = ntohl(*msgp++);
  unemployment_cost = ntohl(*msgp++);
  transport_cost = ntohl(*msgp++);
  import_cost = ntohl(*msgp++);
  ly_interest = ntohl(*msgp++);
  ly_school_cost = ntohl(*msgp++);
  ly_university_cost = ntohl(*msgp++);
  ly_deaths_cost = ntohl(*msgp++);
  ly_windmill_cost = ntohl(*msgp++);
  ly_recycle_cost = ntohl(*msgp++);
  ly_health_cost = ntohl(*msgp++);
  ly_rocket_pad_cost = ntohl(*msgp++);
  ly_fire_cost = ntohl(*msgp++);
  ly_cricket_cost = ntohl(*msgp++);

  update_scoreboard.yearly_1 = 1;
  update_scoreboard.yearly_2 = 1;
  update_scoreboard.money = 1;
}

void 
handle_msg_map_types_region (void)
{
  int x, y, xx, yy, size;
  MsgBuf* msg = &recvbuf;
  Int16* msgp16 = (Int16*) MSG_BODY(msg);
  Int32* msgp32 = (Int32*) MSG_BODY(msg);

  printf ("Client received MSG_MAP_TYPES_REGION.\n");
  x = ntohl(*msgp32++);
  y = ntohl(*msgp32++);
  size = ntohl(*msgp32++);
  msgp16 = (Int16*) msgp32;
  for (yy = y; yy < y + size; yy++) {
    for (xx = x; xx < x + size; xx++) {
      MP_TYPE(xx,yy) = ntohs(*msgp16++);
      if (MP_TYPE(xx,yy) != CST_USED) {
	set_mappoint (xx, yy, MP_TYPE(xx,yy));
      }
    }
  }
}

void 
handle_msg_main_scrn_data (void)
{
  int i, x, y, num_entries;
  MsgBuf* msg = &recvbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  printf ("Client received MSG_MAIN_SCRN_DATA.\n");
  num_entries = ntohl(*msgp++);
  printf ("num_entries = %d\n", num_entries);
  for (i = 0; i < num_entries; i++) {
    x = ntohl(*msgp++);
    y = ntohl(*msgp++);
    MP_INFO(x,y).int_1 = ntohl(*msgp++);
    MP_INFO(x,y).int_2 = ntohl(*msgp++);
    MP_INFO(x,y).int_3 = ntohl(*msgp++);
    MP_INFO(x,y).int_4 = ntohl(*msgp++);
    MP_INFO(x,y).int_5 = ntohl(*msgp++);
    MP_INFO(x,y).int_6 = ntohl(*msgp++);
    MP_INFO(x,y).int_7 = ntohl(*msgp++);
    MP_POL(x,y) = ntohl(*msgp++);
  }
  /*  update_scoreboard.mps = 1; */
}

void 
handle_msg_flags_changed (void)
{
  int x, y;
  MsgBuf* msg = &recvbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  printf ("Client received MSG_FLAGS_CHANGED\n");
  x = ntohl(*msgp++);
  y = ntohl(*msgp++);
  MP_INFO(x,y).flags = ntohl(*msgp++);

  if (mcbx == x && mcby == y) {
      if (market_cb_flag) {
	  draw_market_cb ();
      } else if (port_cb_flag) {
	  draw_port_cb ();
      }
  }
}

void 
handle_msg_mini_data (void)
{
    int i, x, y;
    MsgBuf* msg = &recvbuf;
    Int16* msgp16 = (Int16*) MSG_BODY(msg);
    Int32* msgp32 = (Int32*) MSG_BODY(msg);
    int mini_type, num_entries;

    printf ("Client received MSG_MINI_DATA.\n");
    mini_type = ntohl(*msgp32++);
    num_entries = ntohl(*msgp32++);
    msgp16 = (Int16*) msgp32;

    switch (mini_type) {
    case MINI_SCREEN_POL_FLAG:
	for (x = 0; x < WORLD_SIDE_LEN; x++)
	    for (y = 0; y < WORLD_SIDE_LEN; y++)
		MP_POL(x,y) = ntohl(*msgp32++);
	break;
    case MINI_SCREEN_FIRE_COVER:
    case MINI_SCREEN_CRICKET_COVER:
    case MINI_SCREEN_HEALTH_COVER:
	for (x = 0; x < WORLD_SIDE_LEN; x++)
	    for (y = 0; y < WORLD_SIDE_LEN; y++)
		MP_INFO(x,y).flags = ntohl(*msgp32++);
	break;
    case MINI_SCREEN_UB40_FLAG:
	for (i = 0; i < num_entries; i++) {
	    x = ntohl(*msgp32++);
	    y = ntohl(*msgp32++);
	    MP_INFO(x,y).int_1 = ntohl(*msgp32++);
	}
	break;
    case MINI_SCREEN_STARVE_FLAG:
	for (i = 0; i < num_entries; i++) {
	    x = ntohl(*msgp32++);
	    y = ntohl(*msgp32++);
	    MP_INFO(x,y).int_2 = ntohl(*msgp32++);
	}
	break;
    case MINI_SCREEN_COAL_FLAG:
	for (x = 0; x < WORLD_SIDE_LEN; x++)
	    for (y = 0; y < WORLD_SIDE_LEN; y++)
		MP_INFO(x,y).coal_reserve = ntohs(*msgp16++);
	break;
    }
    update_scoreboard.mini = 1;
}

void 
handle_msg_coal_survey_done (void)
{
    MsgBuf* msg = &recvbuf;
    Int32* msgp = (Int32*) MSG_BODY(msg);

    printf ("Client received MSG_COAL_SURVEY_DONE\n");
    total_money = ntohl(*msgp++);
    print_total_money ();
    if (!coal_survey_done) {
	coal_survey_done = 1;
	ok_dial_box ("didcoalsurvey.mes", GOOD, 0L);
    }
}

void 
handle_msg_rocket_built (void)
{
    int x, y;
    MsgBuf* msg = &recvbuf;
    Int32* msgp = (Int32*) MSG_BODY(msg);

    printf ("Client received MSG_ROCKET_BUILT\n");
    x = ntohl(*msgp++);
    y = ntohl(*msgp++);
    if (ask_launch_rocket_now (x,y)) {
	network_launch_rocket (x, y);
    }
}

void 
handle_msg_rocket_fired (void)
{
    int x, y, result;
    MsgBuf* msg = &recvbuf;
    Int32* msgp = (Int32*) MSG_BODY(msg);

    printf ("Client received MSG_ROCKET_FIRED\n");
    x = ntohl(*msgp++);
    y = ntohl(*msgp++);
    result = ntohl(*msgp++);
    MP_TYPE(x,y) = CST_ROCKET_FLOWN;
    display_rocket_result_dialog (result);
}
