/* ---------------------------------------------------------------------- *
 * sernet.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- *
 * Adapted from the freeciv code.
 * Copyrights retained -- add these here.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "lcstring.h"
#include <fcntl.h>

#if !defined (WIN32)
#include <sys/signal.h>
#endif
#include <sys/types.h>
#include <time.h>
#if defined (HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#include <errno.h>
#if !defined (WIN32)
#include <pwd.h>
#endif

#if defined(AIX) || defined(__EMX__)
#include <sys/select.h>
#endif

#include <signal.h>

#if defined (WIN32)
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netdb.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "sernet.h"
#include "shrnet.h"

struct connection connections[MAX_CONNECTIONS];
lc_sock sock;
unsigned short port=DEFAULT_SOCK_PORT;
extern int errno;

#define MAX(x,y) ((x) > (y) ? (x) : (y))


/***************************************************************
...
***************************************************************/
char *mystrerror(int errnum)
{
#if defined(HAVE_STRERROR) || !defined(HAVE_CONFIG_H)
  /* if we have real strerror() or if we're not using configure */
  return strerror(errnum);
#else
  static char buf[64];
  sprintf(buf, "error %d (compiled without strerror)", errnum);
  return buf;
#endif
}


/*****************************************************************************/
void close_connection(struct connection *pconn)
{
#if defined (WIN32)
    closesocket(pconn->sock);
#else
    close(pconn->sock);
#endif
    pconn->used=0;
}

/*****************************************************************************/
void close_connections_and_socket(void)
{
    int i;
  
    for(i=0; i<MAX_CONNECTIONS; i++) {
	if(connections[i].used) {
	    close_connection(&connections[i]);
	}
    }
#if defined (WIN32)
    closesocket(sock);
#else
    close(sock);
#endif
}

/*****************************************************************************
Get and handle:
- new connections,
- input from connections,
- input from server operator in stdin
Returns:
  0 if went past end-of-turn timeout
  2 if force_end_of_sniff found to be set
  1 otherwise (got and processed something?)
*****************************************************************************/
int
sniff_packets(void)
{
    int i;
    int max_desc;
    fd_set readfs;
    struct timeval tv;
    static time_t time_at_turn_end;
    static int year;
    int rv;
  
#if defined (commentout)
    tv.tv_sec=0; tv.tv_usec=0;
    tv.tv_sec=1; tv.tv_usec=0;
#endif
    tv.tv_sec=0; tv.tv_usec=0;
    
    FD_ZERO(&readfs);
#if defined (commentout)
    FD_SET(0, &readfs);
#endif
    FD_SET(sock, &readfs);

#if defined (WIN32)
    max_desc = 0;    /* W32 ignores first argument to select() */
#else
    max_desc=sock;
#endif
    for(i=0; i<MAX_CONNECTIONS; i++) {
	if(connections[i].used) {
	    FD_SET(connections[i].sock, &readfs);
	}
#if !defined (WIN32)
	max_desc=MAX(connections[i].sock, max_desc);
#endif
    }

    rv = select(max_desc+1, &readfs, NULL, NULL, &tv);

    if(rv==0) {
	/* timeout */
    }
#if defined (WIN32)
    if (rv == SOCKET_ERROR) {
	printf ("Got a SOCKET_ERROR from select()!!!\n");
    }
#endif

    /* new players connects */
    if(FD_ISSET(sock, &readfs)) {
	printf ("got new connection\n");
	if(server_accept_connection(sock)==-1)
	    printf("failed accepting connection\n");
    }

#if defined (commentout)
    /* input from server operator */
    else if(FD_ISSET(0, &readfs)) {
	int didget;
	char buf[BUF_SIZE+1];
      
	if((didget=read(0, buf, BUF_SIZE))==-1) {
	    printf ("read from stdin failed\n");
	    exit(1);
	}
	*(buf+didget)='\0';
	printf ("Prompt garbage\n");
    }
#endif

    /* input from a player */
    else {
	for(i=0; i<MAX_CONNECTIONS; i++) {
	    if(connections[i].used && FD_ISSET(connections[i].sock, &readfs)) {
		int rv;
		rv = net_recv_message (connections[i].sock, 
				       &connections[i].msgbuf);
		if (rv < 0) {
		    printf ("Lost connection to player...\n");
		    close_connection(&connections[i]);
		    break;
		} else if (rv > 0) {
		    server_handle_message (&connections[i]);
		}
	    }
	}
    }
    return 1;
}
  
static void  
initialize_client_data (Client_Data* cl)
{
    strcpy (cl->player, "");
    cl->mps_flag = 0;
    cl->main_scrn_flag = 0;
    cl->mini_scrn_flag = 0;
}

/********************************************************************
 server accepts connections from client
********************************************************************/
int server_accept_connection(lc_sock sockfd)
{
    unsigned int fromlen;
    struct sockaddr_in fromend;
    struct hostent *from;
	lc_sock new_sock;
#if defined (WIN32)
    unsigned long nonblocking_arg = 1;
#else
    int flags, rv;
#endif

    fromlen = sizeof fromend;

    new_sock = accept(sockfd, (struct sockaddr *) &fromend, &fromlen);

    /* Set socket to non-blocking */
#if defined (WIN32)
    ioctlsocket (new_sock, FIONBIO, &nonblocking_arg);
#else
    flags = fcntl (new_sock, F_GETFL);
    if (flags==-1) {
	printf ("Error returned from fcntl(F_GETFL)\n");
	return -1;
    }
    rv = fcntl (new_sock, F_SETFL, flags | O_NONBLOCK);
    if (rv==-1) {
	printf ("Error returned from fcntl(F_SETFL)\n");
	return -1;
    }
#endif

    from=gethostbyaddr((char*)&fromend.sin_addr, 
		       sizeof(sizeof(fromend.sin_addr)), 
		       AF_INET);

    if(new_sock!=-1) {
	int i;
	for(i=0; i<MAX_CONNECTIONS; i++) {
	    if(!connections[i].used) {
		connections[i].used=1;
		connections[i].sock=new_sock;
		connections[i].msgbuf.num_bytes = 0;
		connections[i].msgbuf.bufp = connections[i].msgbuf.data;
		initialize_client_data (&connections[i].cldat);

		if(from) {
		    strncpy(connections[i].addr, from->h_name, ADDR_LENGTH);
		    connections[i].addr[ADDR_LENGTH-1]='\0';
		}
		else {
		    strcpy(connections[i].addr, "unknown");
		}
		printf ("connection from %s\n", connections[i].addr);
		return 0;
	    }
	}
	printf ("maximum number of connections reached\n");
	return -1;
    }
    return -1;
}



/********************************************************************
 open server socket to be used to accept client connections
********************************************************************/
int server_open_socket(void)
{
  /* setup socket address */
  struct sockaddr_in src;
  int opt;

  src.sin_addr.s_addr = INADDR_ANY;
  src.sin_family = AF_INET;
  src.sin_port = htons(port);


#if !defined (WIN32)
  signal (SIGPIPE, SIG_IGN);    /* broken pipes are ignored. */
#endif

#if defined (WIN32)
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    printf ("socket failed: %s\n", mystrerror(errno));
    exit(1);
  }
#else
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf ("socket failed: %s\n", mystrerror(errno));
    exit(1);
  }
#endif

  opt=1; 
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt))) {
    printf ("setsockopt failed: %s\n", mystrerror(errno));
  }

  if(bind(sock, (struct sockaddr *) &src, sizeof (src)) < 0) {
    printf ("bind failed: %s\n", mystrerror(errno));
    exit(1);
  }

  if(listen(sock, MAX_CONNECTIONS) < 0) {
    printf ("listen failed: %s\n", mystrerror(errno));
    exit(1);
  }

  return 0;
}


/********************************************************************
...
********************************************************************/
void init_connections(void)
{
  int i;
  for(i=0; i<MAX_CONNECTIONS; i++) { 
    connections[i].used=0;
    connections[i].msgbuf.num_bytes = 0;
    connections[i].msgbuf.bufp = connections[i].msgbuf.data;
    initialize_client_data (&connections[i].cldat);
  }
}
