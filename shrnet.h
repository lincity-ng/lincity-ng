/* ---------------------------------------------------------------------- *
 * shrnet.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __shrnet_h__
#define __shrnet_h__

#if defined (WIN32)
#include <winsock.h>    /* for SOCKET */
#endif
#ifdef __EMX__
#include <sys/types.h>
#include <netinet/in.h> /* htonl(), etc. */
#endif
#include "protocol.h"


/* ---------------------------------------------------------------------- *
 * Type Definitions
 * ---------------------------------------------------------------------- */
#if defined (WIN32)
typedef SOCKET lc_sock;
typedef unsigned short u_short;
#else
typedef int lc_sock;
#endif

/* ---------------------------------------------------------------------- *
 * Public Fn Prototypes
 * ---------------------------------------------------------------------- */
int net_init_interface (void);
int net_recv_message (lc_sock sockid, MsgBuf *msg);
int net_send_message (lc_sock sockid, MsgBuf* msg);

#endif	/* __shrnet_h__ */
