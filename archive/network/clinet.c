/* ---------------------------------------------------------------------- *
 * clinet.c
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
#include <errno.h>
#if defined(AIX) || defined(__EMX__)
#include <sys/select.h>
#endif
#include <signal.h>
#if !defined (WIN32)
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#if defined (WIN32)
#include <winsock.h>
#endif
#include "shrnet.h"
#include "clinet.h"
#include "climsg.h"

/* ---------------------------------------------------------------------- *
 * External Global Variables
 * ---------------------------------------------------------------------- */
extern int errno;

/* ---------------------------------------------------------------------- *
 * Public Global Variables
 * ---------------------------------------------------------------------- */
int network_game = 0;
lc_sock sock;
MsgBuf recvbuf;

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
void join_game(void);
void net_setup(char* hostname, unsigned short port);

/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */

void
join_network_game (char* host, unsigned short port)
{
  if (host == NULL) {
    host = DEFAULT_SOCK_HOST;
  }
  if (port == 0) {
    port = DEFAULT_SOCK_PORT;
  }

  recvbuf.num_bytes = 0;
  recvbuf.bufp = recvbuf.data;

  network_game = 1;
  net_setup (host, port);
  network_join_game ();
}

int 
net_handle_messages(void)
{
  fd_set readfs;
  struct timeval tv;
  int rv;

#if defined (commentout)
  tv.tv_sec=0; tv.tv_usec=0;
  tv.tv_sec=1; tv.tv_usec=0;
#endif
  tv.tv_sec=0; tv.tv_usec=0;

  /*  MY_FD_ZERO(&readfs); */
  FD_ZERO(&readfs);
  FD_SET(sock, &readfs);
  rv = select(sock+1, &readfs, NULL, NULL, &tv);
  if(rv==0) {
    return 0;
  }
  if (rv < 0) {
    printf ("Select failed...but why?\n");
    exit (-1);
  }
  if(FD_ISSET(sock, &readfs)) {
    int rv;
    rv = net_recv_message (sock, &recvbuf);
    if (rv < 0) {
#if defined (commentout)
      printf ("Lost connection to server...\n");
      exit (-1);
#endif
    } else if (rv > 0) {
      client_handle_message ();
      return 1;
    }
  }
  printf ("FD should have been set, but wasn't...\n");
  return -1;
}

/* ---------------------------------------------------------------------- *
 * Private Functions
 * ---------------------------------------------------------------------- */

void
net_setup(char* hostname, unsigned short port)
{
    struct sockaddr_in serv_addr;
    struct hostent *hp;

    if (!net_init_interface ()) {
	exit(2);
    }

    /* GCS FIX: The freeciv guys have some extra code that checks if 
       a numeric IP address was specified (below).
    */
#if defined (commentout)
    if(isdigit((size_t)*hostname)) {
      if((address = inet_addr(hostname)) == -1) {
	strcpy(errbuf, "Invalid hostname");
	return -1;
      }
      src.sin_addr.s_addr = address;
      src.sin_family = AF_INET;
    }
#endif

    /* Lookup the address for the specified host */
    hp = gethostbyname(hostname);
    if (hp == 0) {
      printf("Unknown Host %s",hostname);
      exit(2);
    }

    memset ((void*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy ((char*)&serv_addr.sin_addr, (char*)hp->h_addr, hp->h_length);
    serv_addr.sin_port = htons(port);

    /* Create a socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { 
	perror("CLIENT socket() failed");
	exit(1);
    }

    /* Connect to server */
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
	perror("CLIENT connect() failed");
	exit(1);
    }

    printf ("CLIENT: Made a connection!\n");
}


void 
netShutdown(void) {
#if defined (WIN32)
    closesocket(sock);
#else
    close(sock);
#endif
}
