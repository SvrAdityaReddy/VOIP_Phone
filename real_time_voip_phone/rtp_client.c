/*******
 *
 * FILE INFO:
 * project:	RTP_lib
 * file:	Rtp_Exemple_Send.c
 * started on:	03/26/03
 * started by:	Cedric Lacroix <lacroix_cedric@yahoo.com>
 *
 *
 * TODO:
 *
 * BUGS:
 *
 * UPDATE INFO:
 * updated on:	05/14/03 17:11:47
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


#include "RTP_lib/Config.h"
#include "RTP_lib/RTP.h"
#include "RTP_lib/Types.h"
#include "RTP_lib/Proto.h"

#define MAX_DATA_SIZE 50

int	main(int argc, char**argv) {
	char buffer[MAX_PAYLOAD_LEN];
	char message[MAX_DATA_SIZE];
	context	cid;
	u_int32	period;
	u_int32	t_inc;
	u_int16	size_read;
	u_int16	last_size_read;
	FILE *fd;

	conx_context_t *coucou = NULL;
	remote_address_t *s	 = NULL;

	// to control number of arguments passed to program
    if(argc!=2) {
        printf("Usage: ./rtp_client <server_ip>\n");
        return 0;
    }

	period = Get_Period_us(PAYLOAD_TYPE);
	last_size_read = 1;
	printf("period: %d\n",period);
	// Init_Socket();
	RTP_Create(&cid);
	RTP_Add_Send_Addr(cid, argv[1], UDP_PORT, 6);

	Set_Extension_Profile(cid, 27);
	Add_Extension(cid, 123456);
	Add_Extension(cid, 654321);
	Add_CRSC(cid, 12569);
	if(!fork()) {
		while (fgets(message,MAX_PAYLOAD_LEN,stdin)!=NULL) {
			RTP_Send(cid, t_inc, 0, PAYLOAD_TYPE, message, strlen(message));
			// printf("last_size_read: %d\n",last_size_read);
			// printf("%s\n",message);
		}
	}
	else {
		
	}
	// while (fgets(message,MAX_PAYLOAD_LEN,stdin)!=NULL) {
	// 	RTP_Send(cid, t_inc, 0, PAYLOAD_TYPE, message, strlen(message));
	// 	// printf("last_size_read: %d\n",last_size_read);
	// 	// printf("%s\n",message);
	// }

	RTP_Destroy(cid);
	Close_Socket();
	return (0);
}
