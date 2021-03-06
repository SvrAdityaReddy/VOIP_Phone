/*******
 * 
 * FILE INFO:
 * project:	udpserv
 * file:	udpserv.c
 * started on:	05/01/03 15:14:26
 * started by:	Julien Dupasquier <jdupasquier@wanadoo.fr>
 * 
 * 
 * TODO:
 * 
 * BUGS:
 * 
 * UPDATE INFO:
 * updated on:	05/15/03 16:18:24
 * updated by:	Julien Dupasquier <jdupasquier@wanadoo.fr>
 * 
 *******/

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <math.h>
#include <errno.h>

#include "RTP_lib/Config.h"
#include "RTP_lib/RTP.h"
#include "RTP_lib/Macros.h"
#include "RTP_lib/Proto.h"

#include "RTP_lib/Rtp_struct_definitions.h"

#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define BUFSIZE 1024

static void	us_serve(struct us *, char **argv);
static void	us_conf(struct us *);
static int us_start(struct us *);
static int us_setup(struct us *us);
static struct us *us_init(int, char **);
// static void	us_event(struct us *us, int cid, int *len, char** argv);

int	us_start(struct us *us) {
    t_listener		*srv;
    int			len;

    srv = us->listeners;
    if ((srv->fd = socket(srv->type, srv->family, 0)) == -1)
        {
          perror("socket");
          exit(EXIT_FAILURE);
        }
  #ifdef HAVE_INET6
      if (srv->family == AF_INET6)
          {
            struct sockaddr_in6 *sin;
            
            MEM_ALLOC(sin);
            sin->sin6_len = sizeof (*sin);
            sin->sin6_addr = IN6_ADDR_ANY;
            sin->sin6_port = htons(srv->port);
            len = sin->sin6_len;
            srv->add = (struct sockaddr *) sin;
          }
      else
  #endif
      {
        struct sockaddr_in *sin;
              
        MEM_ALLOC(sin);
        //sin->sin_len = sizeof (*sin);
        sin->sin_addr.s_addr = INADDR_ANY;
        sin->sin_port = htons(srv->port);
        srv->add = (struct sockaddr *) sin;
        srv->len = sizeof(*sin); 
      }
    if ((bind(srv->fd, srv->add, srv->len)) == -1)
      {
        perror("bind");
      exit(EXIT_FAILURE);	  
      }
}


void us_conf(struct us *us) {
	t_listener		*srv;
	t_client		*client;


	/* Listen Config */
	MEM_ALLOC(srv);
	srv->type = SOCK_DGRAM;
	srv->family = AF_INET;
	srv->port = UDP_PORT;
	us->listeners =  srv;

	#ifdef HAVE_INET6
	MEM_ALLOC(srv);
	srv->type = SOCK_DGRAM;
	srv->family = AF_INET6;
	srv->port = UDP_PORT + 2;
	us->listeners = srv;
	#endif
	
	/* Client Config */
	MEM_ALLOC(client);
	srv->clients = client;
	#ifdef HAVE_INET6
	if (srv->type == AF_INET6)
		client->len = sizeof(struct sockaddr_in6);
	#endif
	if (srv->type == AF_INET)
		client->len = sizeof(struct sockaddr_in);
	MEM_SALLOC(client->add, client->len);
	}

struct us *us_init(int ac, char **argv) {
	struct us	*us;
	MEM_ALLOC(us);
	us_conf(us); 
	(void)us_start(us);
	return (us);
}

void us_event(struct us *us, int cid, int *len, char **argv) {
	char msg[MAX_PACKET_LEN];
	t_listener *srv;
	t_client *client;

	static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 8000,
        .channels = 2
    };
    pa_simple *s_s = NULL;
    int ret = 1;
    int error;
	
	srv = us->listeners;
	client = srv->clients;

	if (!(s_s = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }

	if (FD_ISSET(srv->fd, &(us->fdset))) {
		uint8_t buf[BUFSIZE];
		RTP_Receive(cid, srv->fd, msg, len, client->add);
		int i=0;
		for(int i=0;i<BUFSIZE;i++) {
			buf[i]=msg[i];
		}
        if (pa_simple_write(s_s, buf,sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
	        goto finish;
        }
	}
	ret = 0;
    finish:
		if (pa_simple_drain(s_s, &error) < 0) {
        	fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        	goto finish;
    	}
        if (s_s)
	       pa_simple_free(s_s);
        return;
}

int	us_setup(struct us *us) {
	int max = 0;
	t_listener *srv;
	
	FD_ZERO(&(us->fdset));
	srv = us->listeners;
	FD_SET(srv->fd, &(us->fdset));
	if (srv->fd > max)
		max = srv->fd;
 	 return ++max;
}

void us_serve(struct us *us, char **argv) {
	int	max;
	int	cid;
	int	len;

	RTP_Create(&cid);
	while (1) {
		max = us_setup(us);
		switch (select(max, &us->fdset, NULL, NULL, NULL))
		{
		case -1:
		if (errno != EINTR)
			{
			perror("select");
			exit (EXIT_FAILURE);
			}
		break;
		case 0:
		perror("j'y comprends rien");
		exit (EXIT_FAILURE);
		default:
		us_event(us, cid, &len, argv);
		}
	}
	RTP_Destroy(cid);
}

int	main(int ac, char **argv) {
	struct us *server;
	server = us_init(ac, argv);
	us_serve(server,argv);
	return 0;
}
