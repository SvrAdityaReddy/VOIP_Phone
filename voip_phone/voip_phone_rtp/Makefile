BIN		= rtp_client
OBJECTS 	= rtp_client.o RTP.o Hdr_Builder.o Network.o Utils.o
BIN2		= rtp_server
OBJECTS2	= rtp_server.o RTP.o Hdr_Builder.o Network.o Utils.o
BIN3	= rtp_client_g711
OBJECTS3 	= rtp_client_g711.o RTP.o Hdr_Builder.o Network.o Utils.o
BIN4		= rtp_server_g711
OBJECTS4	= rtp_server_g711.o RTP.o Hdr_Builder.o Network.o Utils.o
CC = gcc
LIBS = -lpulse -lpulse-simple

all: send receive send_g711 receive_g711

send: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $(BIN)

send_g711: $(OBJECTS3)
	$(CC) $(OBJECTS3) $(LIBS) -o $(BIN3)

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
	$(CC) $(OBJECTS2) $(LIBS) -o $(BIN2)

receive_g711: $(OBJECTS4)
	$(CC) $(OBJECTS4) $(LIBS) -o $(BIN4)

rtp_server.o: rtp_server.c
	$(CC) -c rtp_server.c

rtp_server_g711.o: rtp_server_g711.c
	$(CC) -c rtp_server_g711.c

rtp_client_g711.o: rtp_client_g711.c
	$(CC) -c rtp_client_g711.c

# Rtp_Exemple_Receive_Print.o: RTP_lib/Rtp_Exemple_Receive_Print.c
# 	$(CC) -c RTP_lib/Rtp_Exemple_Receive_Print.c

clean:
	rm -f $(OBJECTS) $(OBJECTS2) $(OBJECTS3) $(OBJECTS4) $(BIN) $(BIN2) $(BIN3) $(BIN4) *~ *.core
