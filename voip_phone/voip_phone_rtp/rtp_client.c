// /*******
//  *
//  * FILE INFO:
//  * project:	RTP_lib
//  * file:	Rtp_Exemple_Send.c
//  * started on:	03/26/03
//  * started by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
//  *
//  *
//  * TODO:
//  *
//  * BUGS:
//  *
//  * UPDATE INFO:
//  * updated on:	05/14/03 17:11:47
//  * updated by:	Julien Dupasquier <jdupasquier@wanadoo.fr>
//  *
//  *******/



#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>


#include "RTP_lib/Config.h"
#include "RTP_lib/RTP.h"
#include "RTP_lib/Types.h"
#include "RTP_lib/Proto.h"

#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#include "g711mit.c"

// #define MAX_DATA_SIZE 50
#define BUFSIZE 1024


int	main(int argc, char**argv) {
	char buffer[BUFSIZE];
	// char message[MAX_DATA_SIZE];
	context	cid;
	u_int32	period;
	u_int32	t_inc;
	u_int16	size_read;
	u_int16	last_size_read;
	FILE *fd;

	static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };
    pa_simple *s_s = NULL;
    int ret = 1;
    int error;

	conx_context_t *coucou = NULL;
	remote_address_t *s	 = NULL;

	// to control number of arguments passed to program
    if(argc!=2) {
        printf("Usage: ./rtp_client <server_ip>\n");
        return 0;
    }

	period = Get_Period_us(PAYLOAD_TYPE);
	last_size_read = 1;
	Init_Socket();
	RTP_Create(&cid);
	RTP_Add_Send_Addr(cid, argv[1], UDP_PORT, 6);

	Set_Extension_Profile(cid, 27);
	Add_Extension(cid, 123456);
	Add_Extension(cid, 654321);
	Add_CRSC(cid, 12569);
	
	if (!(s_s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
            fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
            goto finish;
        }
    for (;;) {
        uint8_t buf[BUFSIZE];
		unsigned char coded[BUFSIZE];
		if (pa_simple_read(s_s, buf, sizeof(buf), &error) < 0) {
            	fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            	goto finish;
        }
		size_read=BUFSIZE;
		t_inc = last_size_read * period;
		last_size_read = size_read;
		int i=0;
		for(i=0;i<BUFSIZE;i++) {
			coded[i]=linear2alaw(buf[i]);
		}
		RTP_Send(cid,t_inc,0,PAYLOAD_TYPE,coded,size_read);
    }
    ret = 0;
    finish:
    if (s_s)
        pa_simple_free(s_s);

	RTP_Destroy(cid);
	Close_Socket();
	return (0);
}

//rtp client side code for voip phone

// #include		<sys/types.h>
// #include		<sys/socket.h>

// #include		<netinet/in.h>
// #include		<arpa/inet.h>

// #include		<stdio.h>
// #include		<stdlib.h>
// #include		<string.h>
// #include		<unistd.h>
// #include		<err.h>


// #include 		"RTP_lib/Config.h"
// #include 		"RTP_lib/RTP.h"
// #include 		"RTP_lib/Types.h"
// #include 		"RTP_lib/Proto.h"
// #include <stdio.h>

// #include <stdlib.h>
// #include <unistd.h>
// #include <errno.h>
// #include <string.h>
// #include <netdb.h>
// #include <sys/types.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include<signal.h>
// #include <arpa/inet.h>

// #ifdef HAVE_CONFIG_H
// #include <config.h>
// #endif

// #include<sys/stat.h>
// #include<fcntl.h>
// #include <sys/timerfd.h>
// #include <time.h>
// #include "g711mit.c"
// #include <stdlib.h>

// #include <stdint.h> 
// #include <pulse/simple.h>
// #include <pulse/error.h>
// #include <pulse/gccmacro.h>

// #define BUFSIZE 1024

// #define handle_error(msg) \
//         do { perror(msg); exit(EXIT_FAILURE); } while (0)
// long unsigned no=0;
// volatile sig_atomic_t keep_going = 1;

// void my_handler_for_sigint(int signumber)//handler to handle SIGINT ctrl+C
// {
//   char ans[2];
//   if (signumber == SIGINT)
//   {
//     printf("received SIGINT\n");
//     printf("Program received a CTRL-C\n");
//     printf("Terminate Y/N : "); 
//     scanf("%s", ans);
//     if (strcmp(ans,"Y") == 0)//terminate if Y
//     {
//        printf("Exiting ....Press any key\n");
// 	//printf("Statistics:\nno of bytes sent in 10-5 s is %lu\n",no);
// 	//printf("the data rate is therefore %lu Megabytes per second",no*10);
//         keep_going = 0;//set a variable and perform cleanup in main
	
// 	exit(0); 
//     }
//     else
//     {
//        printf("Continung ..\n");
//     }
//   }
// }



// int main(int argc, char *argv[])
// {
// 	char		buffer[MAX_PAYLOAD_LEN];
// 	context		cid;
// 	u_int32		period;
// 	u_int32		t_inc;
// 	u_int16		size_read;
// 	u_int16		last_size_read;
	
	
// 	   static const pa_sample_spec ss = {
//         .format = PA_SAMPLE_S16LE,
//         .rate = 8000,
//         .channels = 2
//     };
//     pa_simple *s1 = NULL;
//     pa_simple *s2=NULL;

// 	conx_context_t 	 *coucou = NULL;
// 	remote_address_t *s	 = NULL;
// 	   int ret = 1;
//     int error;

//     int fd;

//     // struct itimerspec new_value;
//     int max_exp, fd2;
//     struct timespec now;
//     uint64_t exp, tot_exp;
//     ssize_t s3;
	
//     if (argc != 2) {//check for format of arguments
//         fprintf(stderr,"usage: server ip\n");
//         exit(1);
//     }
	
// //    if (clock_gettime(CLOCK_REALTIME, &now) == -1)
//         // handle_error("clock_gettime");

//    /* Create a CLOCK_REALTIME absolute timer with initial
//        expiration and interval as specified in command line */

//    //new_value.it_value.tv_sec = now.tv_sec ;
//     //new_value.it_value.tv_nsec = now.tv_nsec;

  
//         //new_value.it_interval.tv_sec=0;
//         //max_exp = 100000000;//no of times to be repeated
//       	//new_value.it_interval.tv_nsec = 1000;//interval in nsec


// 	period = Get_Period_us(PAYLOAD_TYPE);
// 	last_size_read = 1;

// 	Init_Socket();
// 	RTP_Create(&cid);
// 	RTP_Add_Send_Addr(cid, argv[1], UDP_PORT, 6);

// 	Set_Extension_Profile(cid, 27);
// 	Add_Extension(cid, 123456);
// 	Add_Extension(cid, 654321);
// 	Add_CRSC(cid, 12569);
	
// 	if (!(s1 = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {//create a stream
//         fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
//         goto finish;
//     	}
//     	printf("creating recording stream\n");
	
//      short buf[BUFSIZE];
// 	 unsigned char buf2[BUFSIZE];

// 	if (signal(SIGINT, my_handler_for_sigint) == SIG_ERR)//register signal handler
//       printf("\ncan't catch SIGINT\n");
	
// //fd2 = timerfd_create(CLOCK_REALTIME, 0);
   
//   //  if (fd2 == -1)
//         //handle_error("timerfd_create");

//    //if (timerfd_settime(fd2, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
//        // handle_error("timerfd_settime");

//     for (;;) {
// 	//if(tot_exp==0)no+=BUFSIZE;
//         //s3 = read(fd2, &exp, sizeof(uint64_t));
//         //if (s3 != sizeof(uint64_t))
//             //handle_error("read");

//         //tot_exp += exp;
        
//         	/* Record some data ... */
//         	if (pa_simple_read(s1, buf, sizeof(buf), &error) < 0) {
//             	fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
//             	goto finish;
//         	}
// 		for(int i=0;i<BUFSIZE;i++)
// 		{size_read=BUFSIZE;
// 		buf2[i]=linear2alaw(buf[i]);}//encode using a law
// 		t_inc = last_size_read * period;
// 		RTP_Send(cid, t_inc, 0, PAYLOAD_TYPE, buf2, size_read);//send using RTP
// 		last_size_read = size_read;
// 			}

// 	RTP_Destroy(cid);
// 	Close_Socket();
// 	return (0);

// finish:

//     if (s1)
//         pa_simple_free(s1);

//     return ret;
// }