/* ---------------------------------------------------------------------- *
 * sermsg.c
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
#include "sernet.h"
#include "shrnet.h"
#include "simulate.h"
#include "engine.h"

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
void handle_msg_unimplemented (Connection* c);
void handle_msg_join (Connection* c);
void handle_msg_new_city (Connection* c);
void handle_msg_place_item (Connection* c);
void handle_msg_bulldoze_item (Connection* c);
void handle_msg_mps_req (Connection* c);
void handle_msg_mps_end (Connection* c);
void handle_msg_main_scrn_req (Connection* c);
void handle_msg_main_scrn_end (Connection* c);
void handle_msg_send_flags (Connection* c);
void handle_msg_mini_scrn_req (Connection* c);
void handle_msg_mini_scrn_end (Connection* c);
void handle_msg_do_coal_survey (Connection* c);
void handle_msg_launch_rocket (Connection* c);
void send_mps (Connection* c);
void send_monthly (Connection* c);
void send_yearly (Connection* c);
void send_main_scrn (Connection* c);
void send_map_types_region (int sockid, int x, int y, int size);
void send_mini_scrn (Connection* c);


/* ---------------------------------------------------------------------- *
 * External Global Variables
 * ---------------------------------------------------------------------- */
extern int real_time;
extern int zoom_originx, zoom_originy;

extern int tpopulation, tstarving_population, tunemployed_population;
extern int tfood_in_markets;

/* ---------------------------------------------------------------------- *
 * Private Global Variables
 * ---------------------------------------------------------------------- */
void (*msg_handlers[NUM_CLIENT_MSG_TYPES])(Connection*) = 
{
    handle_msg_join,
    handle_msg_new_city,
    handle_msg_unimplemented,
    handle_msg_unimplemented,
    handle_msg_place_item,
    handle_msg_bulldoze_item,
    handle_msg_mps_req,
    handle_msg_mps_end,
    handle_msg_main_scrn_req,
    handle_msg_main_scrn_end,
    handle_msg_send_flags,
    handle_msg_mini_scrn_req,
    handle_msg_mini_scrn_end,
    handle_msg_do_coal_survey,
    handle_msg_launch_rocket
};


/* ---------------------------------------------------------------------- *
 * Public Functions -- Incoming messages
 * ---------------------------------------------------------------------- */
void 
server_handle_message (Connection* c)
{
  MsgBuf* msg = &c->msgbuf;
  if (MSG_OPCODE(msg) >= CLIENT_MSG_MIN && 
      MSG_OPCODE(msg) <= CLIENT_MSG_MAX) {
    msg_handlers[MSG_OPCODE(msg)-CLIENT_MSG_MIN](c);
  } else {
    printf ("Warning. Server received an ill-formed message (type=%d)\n",
	    MSG_OPCODE(msg));
  }
}


/* ---------------------------------------------------------------------- *
 * Public Functions -- Outgoing messages
 * ---------------------------------------------------------------------- */
void
send_periodic_messages (void)
{
    static int mappoint_stats_time = 0;
    static int mini_screen_time = 0;
    int i;

    for(i=0; i<MAX_CONNECTIONS; i++) {
	if(connections[i].used) {
	    if ((real_time > mappoint_stats_time) &&
		(connections[i].cldat.main_scrn_flag)) {
		send_main_scrn (&connections[i]);
	    }
	    if ((real_time > mappoint_stats_time) &&
		(connections[i].cldat.mps_flag)) {
		send_mps (&connections[i]);
	    }
	    if (total_time % NUMOF_DAYS_IN_MONTH == (NUMOF_DAYS_IN_MONTH-1)) {
		send_monthly (&connections[i]);
	    }
	    if (total_time % NUMOF_DAYS_IN_YEAR == (NUMOF_DAYS_IN_YEAR-1)) {
		send_yearly (&connections[i]);
	    }
	    if ((real_time > mini_screen_time) &&
		(connections[i].cldat.mini_scrn_flag)) {
		send_mini_scrn (&connections[i]);
	    }
	}
    }
    if (real_time > mappoint_stats_time) {
	mappoint_stats_time = real_time + 1000;
    }
    if (real_time > mini_screen_time) {
	mini_screen_time = real_time + 1000;
    }
}

void 
send_map_types (int sockid)
{
  int x, y;
  MsgBuf msg;
  Int16* msgp = (Int16*) MSG_BODY(&msg);
  MSG_OPCODE(&msg) = MSG_MAP_TYPES;
  MSG_LENGTH(&msg) = WORLD_SIDE_LEN * WORLD_SIDE_LEN * sizeof(Int16);
  for (y = 0; y < WORLD_SIDE_LEN; y++)
    for (x = 0; x < WORLD_SIDE_LEN; x++)
      *msgp++ = htons(MP_TYPE(x,y));
  net_send_message (sockid, &msg);
  printf ("Server sent MSG_MAP_TYPES\n");
}

void 
broadcast_map_types_region (int x, int y, int size)
{
    int i;
    for(i=0; i<MAX_CONNECTIONS; i++) {
	if(connections[i].used) {
	    send_map_types_region (connections[i].sock, x, y, size);
	}
    }
}

void 
send_map_types_region (int sockid, int x, int y, int size)
{
  int xx, yy;
  MsgBuf msg;
  Int16* msgp16 = (Int16*) MSG_BODY(&msg);
  Int32* msgp32 = (Int32*) MSG_BODY(&msg);
  MSG_OPCODE(&msg) = MSG_MAP_TYPES_REGION;
  MSG_LENGTH(&msg) = 3 * sizeof(Int32) + size * size * sizeof(Int16);
  *msgp32++ = htonl(x);
  *msgp32++ = htonl(y);
  *msgp32++ = htonl(size);
  msgp16 = (Int16*) msgp32;
  for (yy = y; yy < y + size; yy++)
    for (xx = x; xx < x + size; xx++)
      *msgp16++ = htons(MP_TYPE(xx,yy));
  net_send_message (sockid, &msg);
  printf ("Server sent MSG_MAP_TYPES_REGION\n");
}

void 
send_map_zoom (int sockid)
{
  MsgBuf msg;
  Int32* msgp = (Int32*) MSG_BODY(&msg);
  MSG_OPCODE(&msg) = MSG_MAP_ZOOM;
  MSG_LENGTH(&msg) = 2 * sizeof(Int32);
  *msgp++ = htonl(zoom_originx);
  *msgp++ = htonl(zoom_originy);
  net_send_message (sockid, &msg);
  printf ("Server sent MSG_MAP_ZOOM (%d,%d)\n", zoom_originx, zoom_originy);
}

void 
send_item_placed (int sockid, int x, int y, int selected_item)
{
  MsgBuf msg;
  Int32* msgp = (Int32*) MSG_BODY(&msg);
  MSG_OPCODE(&msg) = MSG_ITEM_PLACED;
  MSG_LENGTH(&msg) = 3 * sizeof(Int32);
  *msgp++ = htonl(x);
  *msgp++ = htonl(y);
  *msgp++ = htonl(selected_item);
  net_send_message (sockid, &msg);
  printf ("Server sent MSG_ITEM_PLACED\n");
}

void 
send_mps (Connection* c)
{
  MsgBuf msg;
  Int32* msgp = (Int32*) MSG_BODY(&msg);
  int x = c->cldat.mps_x;
  int y = c->cldat.mps_y;

  MSG_OPCODE(&msg) = MSG_MPS_DATA;
  MSG_LENGTH(&msg) = 12 * sizeof(Int32);
  *msgp++ = htonl(x);
  *msgp++ = htonl(y);
  *msgp++ = htonl(MP_INFO(x,y).population);
  *msgp++ = htonl(MP_INFO(x,y).flags);
  *msgp++ = htonl(MP_INFO(x,y).int_1);
  *msgp++ = htonl(MP_INFO(x,y).int_2);
  *msgp++ = htonl(MP_INFO(x,y).int_3);
  *msgp++ = htonl(MP_INFO(x,y).int_4);
  *msgp++ = htonl(MP_INFO(x,y).int_5);
  *msgp++ = htonl(MP_INFO(x,y).int_6);
  *msgp++ = htonl(MP_INFO(x,y).int_7);
  *msgp++ = htonl(MP_POL(x,y));

  net_send_message (c->sock, &msg);
  printf ("Server sent MSG_MPS_DATA\n");
}

void 
send_main_scrn (Connection* c)
{
  MsgBuf msg;
  Int32* msgp = (Int32*) MSG_BODY(&msg);
  int xx, yy;
  int x = c->cldat.main_scrn_x;
  int y = c->cldat.main_scrn_y;
  int w = c->cldat.main_scrn_w;
  int h = c->cldat.main_scrn_h;
  int num_entries = 0;

  MSG_OPCODE(&msg) = MSG_MAIN_SCRN_DATA;

  msgp++;  /* Skip past num_entries field */
  for (xx = x; xx < x + w; xx++) {
    for (yy = y; yy < y + h; yy++) {
      short t = MP_TYPE(xx,yy);
      if (t == CST_USED || t == CST_GREEN)
	continue;
      *msgp++ = htonl(xx);
      *msgp++ = htonl(yy);
      *msgp++ = htonl(MP_INFO(xx,yy).int_1);
      *msgp++ = htonl(MP_INFO(xx,yy).int_2);
      *msgp++ = htonl(MP_INFO(xx,yy).int_3);
      *msgp++ = htonl(MP_INFO(xx,yy).int_4);
      *msgp++ = htonl(MP_INFO(xx,yy).int_5);
      *msgp++ = htonl(MP_INFO(xx,yy).int_6);
      *msgp++ = htonl(MP_INFO(xx,yy).int_7);
      *msgp++ = htonl(MP_POL(xx,yy));
      num_entries ++;
    }
  }

  /* Fill in num_entries */
  msgp = (Int32*) MSG_BODY(&msg);
  *msgp = htonl(num_entries);
  MSG_LENGTH(&msg) = (1 + num_entries * 10) * sizeof(Int32);

  net_send_message (c->sock, &msg);
  printf ("Server sent MSG_MAIN_SCRN_DATA\n");
}

void 
send_monthly (Connection* c)
{
  MsgBuf msg;
  Int32* msgp = (Int32*) MSG_BODY(&msg);

  MSG_OPCODE(&msg) = MSG_MONTHLY;
  MSG_LENGTH(&msg) = 14 * sizeof(Int32);
  *msgp++ = htonl(food_in_markets);       /* Is this needed? */
  *msgp++ = htonl(housed_population);
  *msgp++ = htonl(numof_shanties);
  *msgp++ = htonl(people_pool);
  *msgp++ = htonl(population);            /* Is this needed? */
  *msgp++ = htonl(rockets_launched);
  *msgp++ = htonl(rockets_launched_success);
  *msgp++ = htonl(tech_level);
  *msgp++ = htonl(tfood_in_markets);
  *msgp++ = htonl(total_time);
  *msgp++ = htonl(tpopulation);
  *msgp++ = htonl(tstarving_population);
  *msgp++ = htonl(tunemployed_population);
  *msgp++ = htonl(unnat_deaths);

  net_send_message (c->sock, &msg);
  printf ("Server sent MSG_MONTHLY\n");
}

void 
send_yearly (Connection* c)
{
  MsgBuf msg;
  Int32* msgp = (Int32*) MSG_BODY(&msg);

  MSG_OPCODE(&msg) = MSG_YEARLY;
  MSG_LENGTH(&msg) = 19 * sizeof(Int32);
  *msgp++ = htonl(total_money);
  *msgp++ = htonl(income_tax);
  *msgp++ = htonl(coal_tax);
  *msgp++ = htonl(goods_tax);
  *msgp++ = htonl(export_tax);
  *msgp++ = htonl(other_cost);
  *msgp++ = htonl(unemployment_cost);
  *msgp++ = htonl(transport_cost);
  *msgp++ = htonl(import_cost);
  *msgp++ = htonl(ly_interest);
  *msgp++ = htonl(ly_school_cost);
  *msgp++ = htonl(ly_university_cost);
  *msgp++ = htonl(ly_deaths_cost);
  *msgp++ = htonl(ly_windmill_cost);
  *msgp++ = htonl(ly_recycle_cost);
  *msgp++ = htonl(ly_health_cost);
  *msgp++ = htonl(ly_rocket_pad_cost);
  *msgp++ = htonl(ly_fire_cost);
  *msgp++ = htonl(ly_cricket_cost);

  net_send_message (c->sock, &msg);
  printf ("Server sent MSG_YEARLY (income_tax = %d)\n", income_tax);
}

void 
send_flags_changed (int sockid, int x, int y)
{
    MsgBuf msg;
    Int32* msgp32 = (Int32*) MSG_BODY(&msg);
    MSG_OPCODE(&msg) = MSG_FLAGS_CHANGED;
    MSG_LENGTH(&msg) = 3 * sizeof(Int32);
    *msgp32++ = htonl(x);
    *msgp32++ = htonl(y);
    *msgp32++ = htonl(MP_INFO(x,y).flags);
    net_send_message (sockid, &msg);
    printf ("Server sent MSG_FLAGS_CHANGED\n");
}

void 
send_mini_scrn (Connection* c)
{
    MsgBuf msg;
    Int16* msgp16 = (Int16*) MSG_BODY(&msg);
    Int32* msgp32 = (Int32*) MSG_BODY(&msg);
    int num_entries = 0;
    int x, y;

    MSG_OPCODE(&msg) = MSG_MINI_SCRN_DATA;

    *msgp32++ = htonl(c->cldat.mini_scrn_type);
    msgp32++;  /* Skip past num_entries field */
    msgp16 = (Int16*) msgp32;

    for (x = 0; x < WORLD_SIDE_LEN; x++) {
	for (y = 0; y < WORLD_SIDE_LEN; y++) {
	    switch (c->cldat.mini_scrn_type) {
	    case MINI_SCREEN_POL_FLAG:
		*msgp32++ = htonl(MP_POL(x,y));
		num_entries ++;
		break;
	    case MINI_SCREEN_FIRE_COVER:
	    case MINI_SCREEN_CRICKET_COVER:
	    case MINI_SCREEN_HEALTH_COVER:
		*msgp32++ = htonl(MP_INFO(x,y).flags);
		num_entries ++;
		break;
	    case MINI_SCREEN_UB40_FLAG:
		if (MP_GROUP_IS_RESIDENCE(x,y)) {
		    *msgp32++ = htonl(x);
		    *msgp32++ = htonl(y);
		    *msgp32++ = htonl(MP_INFO(x,y).int_1);
		    num_entries ++;
		}
		break;
	    case MINI_SCREEN_STARVE_FLAG:
		if (MP_GROUP_IS_RESIDENCE(x,y)) {
		    *msgp32++ = htonl(x);
		    *msgp32++ = htonl(y);
		    *msgp32++ = htonl(MP_INFO(x,y).int_2);
		    num_entries ++;
		}
		break;
	    case MINI_SCREEN_COAL_FLAG:
		*msgp16++ = htons(MP_INFO(x,y).coal_reserve);
		num_entries ++;
		break;
	    }
	}
    }

    /* Fill in num_entries */
    msgp32 = (Int32*) MSG_BODY(&msg);
    msgp32++;
    *msgp32 = htonl(num_entries);
    switch (c->cldat.mini_scrn_type) {
    case MINI_SCREEN_POL_FLAG:
    case MINI_SCREEN_FIRE_COVER:
    case MINI_SCREEN_CRICKET_COVER:
    case MINI_SCREEN_HEALTH_COVER:
	MSG_LENGTH(&msg) = (2 + num_entries) * sizeof(Int32);
	break;
    case MINI_SCREEN_UB40_FLAG:
    case MINI_SCREEN_STARVE_FLAG:
	MSG_LENGTH(&msg) = (2 + 3 * num_entries) * sizeof(Int32);
	break;
    case MINI_SCREEN_COAL_FLAG:
	MSG_LENGTH(&msg) = (2 * sizeof(Int32)) + (num_entries * sizeof(Int16));
	break;
    }

    net_send_message (c->sock, &msg);
    printf ("Server sent MSG_MINI_SCRN_DATA\n");
}

void 
send_coal_survey_done (int sockid)
{
    MsgBuf msg;
    Int32* msgp32 = (Int32*) MSG_BODY(&msg);
    MSG_OPCODE(&msg) = MSG_COAL_SURVEY_DONE;
    MSG_LENGTH(&msg) = 1 * sizeof(Int32);
    *msgp32++ = htonl(total_money);
    net_send_message (sockid, &msg);
    printf ("Server sent MSG_COAL_SURVEY_DONE\n");
}

void
send_rocket_built (int sockid, int x, int y)
{
    MsgBuf msg;
    Int32* msgp32 = (Int32*) MSG_BODY(&msg);
    MSG_OPCODE(&msg) = MSG_ROCKET_BUILT;
    MSG_LENGTH(&msg) = 2 * sizeof(Int32);
    *msgp32++ = htonl(x);
    *msgp32++ = htonl(y);
    net_send_message (sockid, &msg);
    printf ("Server sent MSG_ROCKET_BUILT\n");
}

void
broadcast_rocket_built (int x, int y)
{
    int i;
    for(i=0; i<MAX_CONNECTIONS; i++) {
	if(connections[i].used) {
	    send_rocket_built (connections[i].sock, x, y);
	}
    }
}

void
send_rocket_fired (int sockid, int x, int y, int result)
{
    MsgBuf msg;
    Int32* msgp32 = (Int32*) MSG_BODY(&msg);
    MSG_OPCODE(&msg) = MSG_ROCKET_FIRED;
    MSG_LENGTH(&msg) = 3 * sizeof(Int32);
    *msgp32++ = htonl(x);
    *msgp32++ = htonl(y);
    *msgp32++ = htonl(result);
    net_send_message (sockid, &msg);
    printf ("Server sent MSG_ROCKET_FIRED\n");
}

void
broadcast_rocket_fired (int x, int y, int result)
{
    int i;
    for(i=0; i<MAX_CONNECTIONS; i++) {
	if(connections[i].used) {
	    send_rocket_fired (connections[i].sock, x, y, result);
	}
    }
}

/* ---------------------------------------------------------------------- *
 * Private Functions -- Incoming messages
 * ---------------------------------------------------------------------- */
void 
handle_msg_unimplemented (Connection* c)
{
    MsgBuf* msg = &c->msgbuf;
    printf ("Warning. Server received unimplemented message type (type=%d)\n",
	    MSG_OPCODE(msg));
}

void 
handle_msg_join (Connection* c)
{
  MsgBuf* msg = &c->msgbuf;
  printf ("Server received MSG_JOIN.\n");

  /* Send entire map to client */
  send_map_types (c->sock);
  /* Tell client where to focus */
  send_map_zoom (c->sock);
}

void 
handle_msg_new_city (Connection* c)
{
  int i;
  MsgBuf* msg = &c->msgbuf;

  printf ("Server received MSG_NEW_CITY.\n");
  engine_new_city (&zoom_originx, &zoom_originy, 1);

  /* Broadcast new city info to all clients */
  for(i=0; i<MAX_CONNECTIONS; i++) {
    if(connections[i].used) {
      /* Send entire map to client */
      send_map_types (connections[i].sock);
      /* Tell client where to focus */
      send_map_zoom (connections[i].sock);
    }
  }
}

void 
handle_msg_place_item (Connection* c)
{
  int i, x, y, rv;
  short selected_item;
  MsgBuf* msg = &c->msgbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  printf ("Server received MSG_PLACE_ITEM.\n");
  x = ntohl(*msgp++);
  y = ntohl(*msgp++);
  selected_item = (short) ntohl(*msgp++);
  if ((rv = engine_place_item (x, y, selected_item)) < 0) {
    /* Unsuccessful engine_place_item().  Tell the client. */
    printf ("Server could not complete place_item() request.\n");
    /* GCS FIX -- Add return code here */
  } else {
    /* Broadcast place item msg to all clients */
    for(i=0; i<MAX_CONNECTIONS; i++) {
      if(connections[i].used) {
	send_item_placed (connections[i].sock, x, y, selected_item);
      }
    }
  }
}

void 
handle_msg_bulldoze_item (Connection* c)
{
  int x, y;
  MsgBuf* msg = &c->msgbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  printf ("Server received MSG_BULLDOZE_ITEM.\n");
  x = ntohl(*msgp++);
  y = ntohl(*msgp++);

  /* The broadcast to other clients is done within engine_bulldoze_item() */
  engine_bulldoze_item (x, y);
}

void 
handle_msg_mps_req (Connection* c)
{
  MsgBuf* msg = &c->msgbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  c->cldat.mps_flag = 1;
  c->cldat.mps_x = ntohl(*msgp++);
  c->cldat.mps_y = ntohl(*msgp++);
  printf ("Server received MSG_MPS_REQ (%d,%d).\n",
	  c->cldat.mps_x, c->cldat.mps_y);
}

void 
handle_msg_mps_end (Connection* c)
{
  MsgBuf* msg = &c->msgbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  printf ("Server received MSG_MPS_END.\n");
  c->cldat.mps_flag = 0;
}


void 
handle_msg_main_scrn_req (Connection* c)
{
  MsgBuf* msg = &c->msgbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  c->cldat.main_scrn_flag = 1;
  c->cldat.main_scrn_x = ntohl(*msgp++);
  c->cldat.main_scrn_y = ntohl(*msgp++);
  c->cldat.main_scrn_w = ntohl(*msgp++);
  c->cldat.main_scrn_h = ntohl(*msgp++);
  printf ("Server received MSG_MAIN_SCRN_REQ (%d,%d,%d,%d).\n",
	  c->cldat.main_scrn_x, c->cldat.main_scrn_y,
	  c->cldat.main_scrn_w, c->cldat.main_scrn_h);
}

void 
handle_msg_main_scrn_end (Connection* c)
{
  MsgBuf* msg = &c->msgbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  printf ("Server received MSG_MAIN_SCRN_END.\n");
  c->cldat.main_scrn_flag = 0;
}

void 
handle_msg_send_flags (Connection* c)
{
    int i, x, y, flags;
    MsgBuf* msg = &c->msgbuf;
    Int32* msgp = (Int32*) MSG_BODY(msg);

    x = ntohl(*msgp++);
    y = ntohl(*msgp++);
    flags = ntohl(*msgp++);
    printf ("Server received MSG_SEND_FLAGS (%d,%d).\n",x,y);

    /* Update flags */
    MP_INFO(x,y).flags = flags;

    /* Broadcast new flags to all clients */
    for(i=0; i<MAX_CONNECTIONS; i++) {
	if(connections[i].used) {
	    send_flags_changed (connections[i].sock, x, y);
	}
    }
}

void 
handle_msg_mini_scrn_req (Connection* c)
{
  MsgBuf* msg = &c->msgbuf;
  Int32* msgp = (Int32*) MSG_BODY(msg);

  c->cldat.mini_scrn_flag = 1;
  c->cldat.mini_scrn_type = ntohl(*msgp++);
  printf ("Server received MSG_MINI_SCRN_REQ (%d).\n",
	  c->cldat.mini_scrn_type);
}

void 
handle_msg_mini_scrn_end (Connection* c)
{
    MsgBuf* msg = &c->msgbuf;
    Int32* msgp = (Int32*) MSG_BODY(msg);

    printf ("Server received MSG_MINI_SCRN_END.\n");
    c->cldat.mini_scrn_flag = 0;
}

void 
handle_msg_do_coal_survey (Connection* c)
{
    int i;
    MsgBuf* msg = &c->msgbuf;
    Int32* msgp = (Int32*) MSG_BODY(msg);

    printf ("Server received MSG_DO_COAL_SURVEY.\n");
    if (!coal_survey_done) {
	engine_do_coal_survey ();
	/* Broadcast to clients */
	for(i=0; i<MAX_CONNECTIONS; i++) {
	    if(connections[i].used) {
		send_coal_survey_done (connections[i].sock);
	    }
	}
    }
}

void 
handle_msg_launch_rocket (Connection* c)
{
    int x, y;
    MsgBuf* msg = &c->msgbuf;
    Int32* msgp = (Int32*) MSG_BODY(msg);

    printf ("Server received MSG_LAUNCH_ROCKET.\n");
    x = ntohl(*msgp++);
    y = ntohl(*msgp++);
    if (MP_TYPE(x,y) >= CST_ROCKET_5 && MP_TYPE(x,y) <= CST_ROCKET_7) {
	/* engine_launch_rocket will propagate results to clients */
	engine_launch_rocket (x,y);
    } else {
	printf ("Server ignored MSG_LAUNCH_ROCKET!!!\n");
    }
}
