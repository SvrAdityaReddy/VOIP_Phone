# VOIP_Phone

In this Project we tried to implement Voice Over Internet Protocol(VOIP) phone using TCP protocol, RTP protocol and pulse audio(used for recording sound and playback of sound).

## Tools used

Type | Name
----------|--------
Development Langauge | C
TCP | Inbuilt API's of Linux
RTP | RTP lib available from sourcefourge

## Methodologies

The following is the way we had implemented VOIP phone.

1). First we had implemented Real Time Text Chat using TCP like whatsapp one to one chat between two people where two people can chat simultaneously. The following are the important api's used to implement Real Time Text Chat using TCP.

``` C

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
void freeaddrinfo(struct addrinfo *res);
int socket(int domain, int type, int protocol);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);

```

2). Next we want to build a VOIP phone. For this we had to record and play back the sound. So for this task we had used "pulseaudio" available for Linux. The following are the important api's used for this purpose.

``` C



```

## Pulse Audio

prec-simple.c, pcat-rec.c are the two example "C" files provided by pulse audio to record and play an pulse audio recorded signal respectively. We can compile them as follow.

``` sh

$ gcc prec-simple.c -lpulse -lpulse-simple -o prec-simple
$ gcc pcat-simple.c -lpulse -lpulse-simple -o pcat-simple

```

# References

