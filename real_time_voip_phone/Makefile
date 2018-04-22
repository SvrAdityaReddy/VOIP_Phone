BIN		= rtp_client
OBJECTS 	= rtp_client.o RTP.o Hdr_Builder.o Network.o Utils.o Rtp_Exemple_Receive_Print.o
BIN2		= rtp_server
OBJECTS2	= rtp_server.o RTP.o Hdr_Builder.o Network.o Utils.o Rtp_Exemple_Receive_Print.o
CC = gcc

all: send receive

send: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(BIN)

rtp_client.o: rtp_client.c
	$(CC) -c rtp_client.c

RTP.o: RTP_lib/RTP.c
	$(CC) -c RTP_lib/RTP.c

Hdr_Builder.o: RTP_lib/Hdr_Builder.c
	$(CC) -c RTP_lib/Hdr_Builder.c

Network.o: RTP_lib/Network.c
	$(CC) -c RTP_lib/Network.c

Utils.o: RTP_lib/Utils.c
	$(CC) -c RTP_lib/Utils.c

receive: $(OBJECTS2)
	$(CC) $(OBJECTS2) -o $(BIN2)

rtp_server.o: rtp_server.c
	$(CC) -c rtp_server.c

Rtp_Exemple_Receive_Print.o: RTP_lib/Rtp_Exemple_Receive_Print.c
	$(CC) -c RTP_lib/Rtp_Exemple_Receive_Print.c

clean:
	rm -f $(OBJECTS) $(OBJECTS2) $(BIN) $(BIN2) *~ *.core
