/* ---------------------------------------------------------------------- *
 * clinet.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __clinet_h__
#define __clinet_h__

#include "shrnet.h"
#include "protocol.h"

extern lc_sock sock;

void join_network_game (char* host, unsigned short port);
int net_handle_messages (void);
void client_handle_message (void);

#endif
