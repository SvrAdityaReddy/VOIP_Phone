# VOIP Phone

In this Project we tried to implement Voice Over Internet Protocol(VOIP) phone using TCP protocol, RTP protocol and pulse audio(used for recording sound and playback of sound).

## Tools used

Type | Name
----------|--------
Development Langauge | C
TCP | Inbuilt API's of Linux
RTP | RTP lib available from sourceforge
Audio record and playback | pulseaudio

## Installation of pulse audio

``` sh

$ sudo apt-get update
$ sudo apt-get install pulseaudio
$ sudo apt-get install libpulse-dev
$ sudo apt-get install libao-dev

```

### Testing of Pulse Audio

prec-simple.c, pcat-rec.c are the two example "C" files provided by pulse audio to record and play an pulse audio recorded signal respectively. We can compile them as follow.

``` sh

$ gcc prec-simple.c -lpulse -lpulse-simple -o prec-simple
$ gcc pcat-simple.c -lpulse -lpulse-simple -o pcat-simple

```

These example files are available on pulseaudio website [1].

## Methodologies

The following is the way we had implemented VOIP phone.

1). First we had implemented Real Time Text Chat using TCP like whatsapp one to one chat between two people where two people can chat simultaneously. The following are the important api's used to implement Real Time Text Chat using TCP [2].

``` C

int getaddrinfo(const char *node, const char *service, 
        const struct addrinfo *hints,struct addrinfo **res);
void freeaddrinfo(struct addrinfo *res);
int socket(int domain, int type, int protocol);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);

```

2). Next we want to build a VOIP phone. For this we had to record and play back the sound. So for this task we had used "pulseaudio" available for Linux. The following are the important api's used for this purpose.

``` C

typedef struct pa_sample_spec {
     pa_sample_format_t format;
     uint32_t rate;
     uint8_t channels;
 } pa_sample_spec;

int pa_simple_read (pa_simple * s, void * data, size_t bytes, int * error); 
int pa_simple_write (pa_simple * s, const void * data, size_t bytes, int * error); 	
int pa_simple_drain (pa_simple * s, int * error);
pa_simple* pa_simple_new (const char * server, const char * name,     
        pa_stream_direction_t dir, const char * dev, const char * stream_name,
        const pa_sample_spec * ss, const pa_channel_map * map,
        const pa_buffer_attr * attr, int * error); 		

```

3). Next we had integrated both pulseaudio and Real Time Text Chat and obtained VOIP phone using TCP protocol. The files we coded are named as "client.c", "server.c". We named it like that because one initiates the connection with the other. These files perform duplex communication.

4). Next we had looked at RTP library in Linux. We had found two libraries. One is "oRTP" from "Belledonne Communications" [3] and other is RTP_lib available from sourceforge website [4]. 

5). Next we had used RTP_lib along with pulseaudio and g711 codec to implement VOIP phone. The files we coded are named as "rtp_client.c", "rtp_server.c". We named it like that because one initiates the connection with the other. These files when run on both sides can perform duplex communication.

The following are the important api's of RTP_lib used 

``` C

int RTP_Create(context *the_context);
int RTP_Send(context cid, u_int32 tsinc, u_int8 marker, u_int16 pti, 
        u_int8 *payload, int len);
int RTP_Add_Send_Addr(context cid, char *addr, u_int16 port, u_int8 ttl);
int RTP_Receive(context cid, int fd, char *payload, int *len, struct sockaddr *sin);
int RTP_Destroy(context cid);

```

## Observations

For very low speed of network both VOIP implementations didnot work well. But for moderate speeds TCP performed better than RTP when using RTP_lib and at higher speeds both performed well.

## Future Scope

We would like to reimplement VOIP phone instead of RTP_lib using oRTP because oRTP has the api's which would get the report statistics of the connection also jitter control and many more which could give real feel of Real Time Control Protocol(RTCP) and Real Time Protocol(RTP). Also, RTP_lib is very higher level implementation. Only problem we faced in using oRTP because of installation problems and installation of it's dependencies which we have to work on.

The code we had written is available on github at https://github.com/SvrAdityaReddy/VOIP_Phone

## References

[1] [Pulseaudio example files](https://freedesktop.org/software/pulseaudio/doxygen/examples.html) <br>

[2] [The Linux Programming Interface by Michael Kerrisk](https://moodle2.units.it/pluginfile.php/115306/mod_resource/content/1/The%20Linux%20Programming%20Interface-Michael%20Kerrisk.pdf) <br>

[3] [oRTP RTP Library from Belledonne Communications](http://www.linphone.org/technical-corner/ortp/overview) <br>

[4] [RTP Library in C available on sourceforge](https://sourceforge.net/projects/rtp-lib/) <br>