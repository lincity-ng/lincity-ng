/* ---------------------------------------------------------------------- *
 * shrnet.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lcstring.h"
#include <errno.h>
#include <sys/types.h>

#if defined (WIN32)
#include <winsock.h>
#endif
#if !defined (WIN32)
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/uio.h>
#endif

#if defined (HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif

#if !defined (WIN32)
#include <pwd.h>
#endif

#ifdef AIX
#include <sys/select.h>
#endif

#include <signal.h>

#if !defined (WIN32)
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "shrnet.h"
#include "protocol.h"

int 
net_init_interface (void)
{
#if defined (WIN32)
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD( 1, 1 );

    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
	/* Tell the user that we couldn't find a useable */
	/* winsock.dll.                                  */
	printf ("Couldn't find a useable winsock.dll\n");
	return 0;
    }

    /* Confirm that the Windows Sockets DLL supports 1.1.*/
    /* Note that if the DLL supports versions greater    */
    /* than 1.1 in addition to 1.1, it will still return */
    /* 1.1 in wVersion since that is the version we      */
    /* requested.                                        */

    if ( LOBYTE( wsaData.wVersion ) != 1 ||
         HIBYTE( wsaData.wVersion ) != 1 ) {
	/* Tell the user that we couldn't find a useable */
	/* winsock.dll.                                  */
	WSACleanup( );
	printf ("Couldn't find a useable winsock.dll\n");
	return 0;
    }

    /* The Windows Sockets DLL is acceptable.  Proceed.  */
#endif

    return 1;
}


/* Returns: 1 if complete message received, 0 if no complete message 
   received, and -1 if error */
int 
net_recv_message(lc_sock sockid, MsgBuf *msg)
{
  int eof = -1;
  int n = 0;

  /* Read header */
  while (msg->num_bytes < MSG_HDR_LEN) {
    n = recv (sockid, msg->bufp, MSG_HDR_LEN - msg->num_bytes, 0);
    if (n < 0) { return n; }
    if (n == 0 && eof) { return eof; }
    eof = 0;
    msg->bufp += n;
    msg->num_bytes += n;
    /* Finished with header? */
    if (msg->num_bytes == MSG_HDR_LEN) {
      MSG_OPCODE(msg) = ntohl(MSG_OPCODE(msg));
      printf ("RECV (0x%x)\n", MSG_OPCODE(msg));
      MSG_LENGTH(msg) = ntohl(MSG_LENGTH(msg));
    }
  }
  
  /* Read body */
  while (msg->num_bytes < MSG_HDR_LEN + MSG_LENGTH(msg)) {
    n = recv (sockid, msg->bufp, 
	      MSG_HDR_LEN + MSG_LENGTH(msg) - msg->num_bytes, 0);
    if (n < 0) { return n; }
    if (n == 0 && eof) { return eof; }
    eof = 0;
    msg->bufp += n;
    msg->num_bytes += n;
  }

  /* Finished with body */
  msg->num_bytes = 0;
  msg->bufp = msg->data;
  return 1;
}


int
net_send_message (lc_sock sockid, MsgBuf* msg)
{
    int rv;
    msg->num_bytes = MSG_HDR_LEN + MSG_LENGTH(msg);
    printf ("SEND (0x%x)\n", MSG_OPCODE(msg));
    MSG_OPCODE(msg) = htonl(MSG_OPCODE(msg));
    MSG_LENGTH(msg) = htonl(MSG_LENGTH(msg));
    rv = send (sockid, msg->data, msg->num_bytes, 0);
    if (rv < 0) {
	printf ("Warning... write error on socket...\n");
    }
    return 0;
}
