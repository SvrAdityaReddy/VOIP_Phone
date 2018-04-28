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

#include "g711mit.c"

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
		unsigned char buf[BUFSIZE];
		uint8_t encoded[BUFSIZE];
		RTP_Receive(cid, srv->fd, msg, len, client->add);
		int i=0;
		for(i=0;i<BUFSIZE;i++) {
			buf[i]=msg[i];
			encoded[i]=alaw2linear(buf[i]);
		}
        if (pa_simple_write(s_s, encoded,sizeof(encoded), &error) < 0) {
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
        return ret;
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

//rtp server side code for voip phone

// #include		<sys/types.h>
// #include		<sys/socket.h>

// #include		<netinet/in.h>
// #include		<arpa/inet.h>

// #include		<stdio.h>
// #include		<stdlib.h>
// #include		<string.h>
// #include		<unistd.h>
// #include		<err.h>
// #include 		<math.h>
// #include		<errno.h>

// #include 		"RTP_lib/Config.h"
// #include 		"RTP_lib/RTP.h"
// #include 		"RTP_lib/Macros.h"
// #include 		"RTP_lib/Proto.h"

// #include		"Rtpserv.h"


// #include <netinet/in.h>
// #include <netdb.h>
// #include <sys/wait.h>
// #include <signal.h>
// #include <sys/timerfd.h>
// #include <time.h>
// #include <stdint.h> 

// #ifdef HAVE_CONFIG_H
// #include <config.h>
// #endif

// #include<sys/types.h>
// #include<sys/stat.h>
// #include<fcntl.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <string.h>
// #include <errno.h>
// #include "g711mit.c"
// #include <pulse/simple.h>
// #include <pulse/error.h>
// #include <pulse/gccmacro.h>

// #define handle_error(msg) \
//         do { perror(msg); exit(EXIT_FAILURE); } while (0)
// #define BUFSIZE 1024

// volatile sig_atomic_t keep_going = 1;

// #define MAXDATASIZE 1024 // max number of bytes we can get at once 

// static void		us_serve(struct us *,int ac,char **av);
// static void		us_conf(struct us *);
// static int		us_start(struct us *);
// static int		us_setup(struct us *us);
// static struct us	*us_init(int, char **);
// static void		us_event(struct us *us, int cid, int *len,int ac,char **av);



// void my_handler_for_sigint(int signumber)//handler for CTRL+C SIGINT
// {
//   char ans[2];
//   if (signumber == SIGINT)
//   {
//     printf("received SIGINT\n");
//     printf("Program received a CTRL-C\n");
//     printf("Terminate Y/N : "); 
//     scanf("%s", ans);
//     if (strcmp(ans,"Y") == 0)//Y for terminate
//     {
//        printf("Exiting ....Press any key\n");
//         keep_going = 0;//atomic variable that controls the server loop is set here;cleanup in main
// 	//kill(getppid(),SIGKILL);
// 	exit(0); 
//     }
//     else
//     {
//        printf("Continung ..\n");//No for continue
//     }
//   }
// }


// void			Print_context(char *msg, int len, int cid)//function that prints context
// {
//   int			i;

//   /*printf("SSRC number                      [%i]\n",context_list[cid]->my_ssrc);
//   printf("Number of packets sent           [%i]\n",context_list[cid]->sending_pkt_count);
//   printf("Number of bytes sent             [%i]\n",context_list[cid]->sending_octet_count);
//   printf("Version                          [%i]\n",context_list[cid]->version);
//   printf("Marker flag                      [%i]\n",context_list[cid]->marker);
//   printf("Padding length                   [%i]\n",context_list[cid]->padding);
//   printf("CSRC length                      [%i]\n",context_list[cid]->CSRClen);
//   printf("Payload type                     [%i]\n",context_list[cid]->pt);
//   for(i = 0; i < context_list[cid]->CSRClen; i++)
//     printf("CSRC list[%i]                     [%li]\n", i, context_list[cid]->CSRCList[i]);
//   printf("First value of timestamp         [%i]\n",context_list[cid]->init_RTP_timestamp);
//   printf("Current value of timestamp       [%i]\n",context_list[cid]->RTP_timestamp);
//   printf("Time elapsed since the beginning [%i]\n",context_list[cid]->time_elapsed);
//   printf("First sequence number            [%i]\n",context_list[cid]->init_seq_no);
//   printf("Current sequence number          [%i]\n",context_list[cid]->seq_no);
//   printf("Extension header Type            [%i]\n",context_list[cid]->hdr_extension->ext_type);
//   printf("Extension header Len             [%i]\n",context_list[cid]->hdr_extension->ext_len);
//   for(i = 0; i < context_list[cid]->hdr_extension->ext_len; i++)
//     printf("Extension header[%i]              [%i]\n", i,context_list[cid]->hdr_extension->hd_ext[i]);*/

//   printf("Message[%i] : [%s]\n\n", len, msg);
// }


// void			print_hdr(rtp_pkt *pkt)//print header and payload
// {
//   /*printf("Header du message :\n");

//   printf("Version       [%d]\n", (pkt->RTP_header->flags & 0xd0) >> 6);
//   printf("Padding       [%d]\n", (pkt->RTP_header->flags & 0x20) >> 5);
//   printf("Ext           [%d]\n", (pkt->RTP_header->flags & 0x10) >> 4);
//   printf("Cc            [%d]\n", (pkt->RTP_header->flags & 0x0f));
//   printf("marker        [%d]\n", (pkt->RTP_header->mk_pt & 0x10) >> 7);
//   printf("PayLoad type  [%d]\n", (pkt->RTP_header->mk_pt & 0x7f));
//   printf("sq_nb         [%i]\n", ntohs(pkt->RTP_header->sq_nb));
//   printf("ts            [%x]\n", ntohl(pkt->RTP_header->ts));
//   printf("ssrc          [%x]\n", ntohl(pkt->RTP_header->ssrc));
//   printf("csrc          [%i]\n", ntohl(pkt->RTP_header->csrc[0]));
//   printf("ext->type     [%i]\n", ntohs(pkt->RTP_extension->ext_type));
//   printf("ext->len      [%i]\n", ntohs(pkt->RTP_extension->ext_len));
//   printf("ext[0]        [%i]\n", ntohl(pkt->RTP_extension->hd_ext[0]));
//   printf("ext[1]        [%i]\n", ntohl(pkt->RTP_extension->hd_ext[1]));
//   printf("len PayLoad   [%i]\n", pkt->payload_len);*/
//   //printf("PayLoad       [%s]\n", pkt->payload);
// }
// int			us_start(struct us *us)
// {
//   t_listener		*srv;
//   int			len;

//   srv = us->listeners;
//   if ((srv->fd = socket(srv->type, srv->family, 0)) == -1)
//     {
//       perror("socket");
//       exit(EXIT_FAILURE);
//     }
// #ifdef HAVE_INET6
//   if (srv->family == AF_INET6)
//     {
//       struct sockaddr_in6 *sin;
      
//       MEM_ALLOC(sin);
//       sin->sin6_len = sizeof (*sin);
//       sin->sin6_addr = IN6_ADDR_ANY;
//       sin->sin6_port = htons(srv->port);
//       len = sin->sin6_len;
//       srv->add = (struct sockaddr *) sin;
//     }
//   else
// #endif
//     {
//       struct sockaddr_in *sin;
            
//       MEM_ALLOC(sin);
//       //sin->sin_len = sizeof (*sin);
//       sin->sin_addr.s_addr = INADDR_ANY;
//       sin->sin_port = htons(srv->port);
//       srv->add = (struct sockaddr *) sin;
//       srv->len = sizeof(*sin); 
//     }
//   if ((bind(srv->fd, srv->add, srv->len)) == -1)
//     {
//       perror("bind");
// 	  exit(EXIT_FAILURE);	  
//     }
// }


// void			us_conf(struct us *us)
// {
//   t_listener		*srv;
//   t_client		*client;


//   /* Listen Config */
//   MEM_ALLOC(srv);
//   srv->type = SOCK_DGRAM;
//   srv->family = AF_INET;
//   srv->port = UDP_PORT;
//   us->listeners =  srv;

// #ifdef HAVE_INET6
//   MEM_ALLOC(srv);
//   srv->type = SOCK_DGRAM;
//   srv->family = AF_INET6;
//   srv->port = UDP_PORT + 2;
//   us->listeners = srv;
// #endif
  
//   /* Client Config */
//   MEM_ALLOC(client);
//   srv->clients = client;
// #ifdef HAVE_INET6
//   if (srv->type == AF_INET6)
//     client->len = sizeof(struct sockaddr_in6);
// #endif
//   if (srv->type == AF_INET)
//     client->len = sizeof(struct sockaddr_in);
//   MEM_SALLOC(client->add, client->len);
// }

// struct us		*us_init(int ac, char **av)
// {
//   struct us		*us;

//   MEM_ALLOC(us);
//    us_conf(us); 
//   (void)us_start(us);
//   return (us);
// }

// void			us_event(struct us *us, int cid, int *len,int ac, char **av)//function that receives rtp packets
// {
//   char			msg[MAX_PACKET_LEN];
//   t_listener		*srv;
//   t_client		*client;

//    static const pa_sample_spec ss = {
//         .format = PA_SAMPLE_S16LE,
//         .rate = 8000,
//         .channels = 2
//     };
//     pa_simple *s1 = NULL;
  
//     int error;
//      int fd;

  
//  if (!(s1 = pa_simple_new(NULL, av[0], PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {//create a stream
//         fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
//         goto finish;
//     }

// unsigned char buf[BUFSIZE];
// short buf2[BUFSIZE];
//   srv = us->listeners;
//   client = srv->clients;
//   if (FD_ISSET(srv->fd, &(us->fdset)))
//     {
     
//       RTP_Receive(cid, srv->fd, msg, len, client->add);//receive
	
//       for(int i=0;i<BUFSIZE;i++)
// 	{buf[i]=msg[i];
// 	buf2[i]=alaw2linear(buf[i]);//decode using a law
// 	//printf("decoding %d %d %d\n",i,buf2[i],buf3[i]);
// 	}

//         /* ... and play it */
//         if (pa_simple_write(s1, buf2, sizeof(buf2), &error) < 0) {
//             fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
//             goto finish;
//         }
//       //msg[*len] = '\0';
//       //Print_context(msg, *len, cid);
//     }

// finish:
//     if (pa_simple_drain(s1, &error) < 0) {
//         fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
//         goto finish;
//     	}
//     if (s1)
//         pa_simple_free(s1);

  
// }

// int			us_setup(struct us *us)
// {
//   int			max = 0;
//   t_listener		*srv;
  
//   FD_ZERO(&(us->fdset));
//   srv = us->listeners;
//   FD_SET(srv->fd, &(us->fdset));
//   if (srv->fd > max)
//     max = srv->fd;
//   return ++max;
// }

// void			us_serve(struct us *us,int ac, char **av)
// {
//   int			max;
//   int			cid;
//   int			len;
//   int max_exp, fd2;
//     struct timespec now;
// struct itimerspec new_value;
//     uint64_t exp, tot_exp;
//     ssize_t s3;

//    //if (clock_gettime(CLOCK_REALTIME, &now) == -1)
//     //    handle_error("clock_gettime");

//    /* Create a CLOCK_REALTIME absolute timer with initial
//        expiration and interval as specified in command line */

//    //new_value.it_value.tv_sec = now.tv_sec ;
//     //new_value.it_value.tv_nsec = now.tv_nsec;

  
//       //  new_value.it_interval.tv_sec=0;
//        // max_exp = 100000000;
//       	//new_value.it_interval.tv_nsec = 1000;
//          //fd2 = timerfd_create(CLOCK_REALTIME, 0);
   
//     //if (fd2 == -1)
//       //  handle_error("timerfd_create");

//   RTP_Create(&cid);
//   while (1)
//     {
//       max = us_setup(us);
//       switch (select(max, &us->fdset, NULL, NULL, NULL))
// 	{
// 	case -1:
// 	  if (errno != EINTR) 
// 	    {
// 	      perror("select");
// 	      exit (EXIT_FAILURE);
// 	    }
// 	  break;
// 	case 0:
// 	  perror("error");
	 
// 	  exit (EXIT_FAILURE);
// 	default:
// 	 {
	
//       printf("server: got connection...\n"); 

//    //if (timerfd_settime(fd2, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
//         //handle_error("timerfd_settime");

//     //for (tot_exp = 0; tot_exp < max_exp;) {
//     //    s3 = read(fd2, &exp, sizeof(uint64_t));
//      //   if (s3 != sizeof(uint64_t))
//        //     handle_error("read");

//         //tot_exp += exp;
// 	  us_event(us, cid, &len,ac,av);//periodically receive packets
// 	}
// 	}
//     }
//   RTP_Destroy(cid);
// }


// int main(int ac, char **av)
// {
//   struct us		*server;
//     if (signal(SIGINT, my_handler_for_sigint) == SIG_ERR)//register signal handler for SIGINT
//       printf("\ncan't catch SIGINT\n");
//   server = us_init(ac, av);
//   us_serve(server,ac,av);
//   return 0;
// }
