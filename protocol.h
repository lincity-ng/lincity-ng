/* ---------------------------------------------------------------------- *
 * protocol.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __protocol_h__
#define __protocol_h__

#include "lcconfig.h"
#include "lin-city.h" /* for WORLD_SIDE_LEN */
#include <sys/types.h>

#define DEFAULT_SOCK_HOST  "localhost"
#define DEFAULT_SOCK_PORT  5139

#define PAK_SIZE    0x100           /* Packet size: constant for simplicity */

#define SEND_RETRIES    5            /* Num. send retries before we give up */
#define TIMEOUT        30         /* Max wait for response before giving up */

#define RV_nOK          0                  /* Network function return codes */
#define RV_nBADSEND     1
#define RV_nBADRECV     2
#define RV_nTIMEOUT     3


/* Client messages */
#define MSG_JOIN              0x1
#define MSG_NEW_CITY          0x2
#define MSG_LOAD_CITY         0x3
#define MSG_SAVE_CITY         0x4
#define MSG_PLACE_ITEM        0x5
#define MSG_BULLDOZE_ITEM     0x6
#define MSG_MPS_REQ           0x7       /* Obsolete */
#define MSG_MPS_END           0x8       /* Obsolete */
#define MSG_MAIN_SCRN_REQ     0x9
#define MSG_MAIN_SCRN_END     0xA
#define MSG_SEND_FLAGS        0xB
#define MSG_MINI_SCRN_REQ     0xC
#define MSG_MINI_SCRN_END     0xD
#define MSG_DO_COAL_SURVEY    0xE
#define MSG_LAUNCH_ROCKET     0xF

#define CLIENT_MSG_MIN        0x1
#define CLIENT_MSG_MAX        0xF
#define NUM_CLIENT_MSG_TYPES  (CLIENT_MSG_MAX - CLIENT_MSG_MIN + 1)

/* Server messages */
#define MSG_MAP_TYPES         0x101
#define MSG_MAP_ZOOM          0x102
#define MSG_ITEM_PLACED       0x103
#define MSG_MPS_DATA          0x104       /* Obsolete */
#define MSG_MONTHLY           0x105
#define MSG_YEARLY            0x106
#define MSG_MAP_TYPES_REGION  0x107
#define MSG_MAIN_SCRN_DATA    0x108
#define MSG_FLAGS_CHANGED     0x109
#define MSG_MINI_SCRN_DATA    0x10A
#define MSG_COAL_SURVEY_DONE  0x10B
#define MSG_ROCKET_BUILT      0x10C
#define MSG_ROCKET_FIRED      0x10D

#define SERVER_MSG_MIN        0x101
#define SERVER_MSG_MAX        0x10D
#define NUM_SERVER_MSG_TYPES  (SERVER_MSG_MAX - SERVER_MSG_MIN + 1)

/* The message consists of an eight byte header, followed by a 
   variable length body.

   +------------------------------------------------------------------+
   |                            opcode                                |
   +------------------------------------------------------------------+
   |                            length                                |
   +------------------------------------------------------------------+
   |                                                                  |
   |                             body                                 |
   |                                                                  |
   +------------------------------------------------------------------+
*/
#define MSG_HDR_LEN 8
#define MAX_DATA_LEN (50 + (WORLD_SIDE_LEN * WORLD_SIDE_LEN * sizeof(Int32)))

#define MSG_OPCODE(msgp) (*((Int32*)(&((msgp)->data[0]))))
#define MSG_LENGTH(msgp) (*((Int32*)(&((msgp)->data[4]))))
#define MSG_BODY(msgp)   (&(msgp)->data[8])

typedef struct msgbuf
{
  int num_bytes;
  char* bufp;
  char data[MSG_HDR_LEN + MAX_DATA_LEN];
} MsgBuf;

#endif  /* __protocol_h__ */
