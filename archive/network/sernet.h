/* ---------------------------------------------------------------------- *
 * sernet.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __sernet_h__
#define __sernet_h__

#include "shrnet.h"
#include "protocol.h"

#define MAX_CONNECTIONS     16
#define BUF_SIZE           512
#define ADDR_LENGTH         32
#define NAME_LEN            32


struct client_data
{
    char player[NAME_LEN];  /* Currently unused */
    int mps_flag;           /* Send mps info if flag == 1 */
    int mps_x;              /* X coord for mps info */
    int mps_y;              /* Y coord for mps info */
    int main_scrn_flag;     /* Send main_scrn info if flag == 1 */
    int main_scrn_x;        /* X coord for client main screen */
    int main_scrn_y;        /* Y coord for client main screen */
    int main_scrn_w;        /* width for client main screen */
    int main_scrn_h;        /* height for client main screen */
    int mini_scrn_flag;     /* Send mini_scrn info if flag == 1 */
    int mini_scrn_type;     /* What type of mini screen (e.g. pollution ) */
};
typedef struct client_data Client_Data;


typedef struct connection
{
  lc_sock sock;
  int used;
  char addr[ADDR_LENGTH];
  MsgBuf msgbuf;
  Client_Data cldat;       /* Server maintains state about the client. */
} Connection;

extern struct connection connections[MAX_CONNECTIONS];

int server_accept_connection(lc_sock sockfd);
int server_open_socket(void);
int sniff_packets(void);
void close_connections_and_socket(void);
void init_connections(void);
void close_connection(struct connection *pconn);
void server_handle_message (Connection* c);

void send_periodic_messages (void);
void broadcast_map_types_region (int x, int y, int size);
void broadcast_rocket_built (int x, int y);

#endif
