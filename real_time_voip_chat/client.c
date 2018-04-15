#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define BUFSIZE 1024

#define MAX_DATA_SIZE 50

int sfd;

void my_signal_handler(int sig_num) {
    char answer[2];
    if(sig_num==SIGINT) {
        printf("\nReceived SIGINT i.e ctrl+c\n");
        printf("Do you want to terminate the process (Y/N): ");
        scanf("%s",answer);
        if(strncmp(answer,"Y",1)==0) {
            close(sfd);
            exit(0);
        }
    }
}

/* A simple routine calling UNIX write() in a loop */
static ssize_t loop_write(int fd, const void*data, size_t size) {
    ssize_t ret = 0;
    while (size > 0) {
        ssize_t r;
        if ((r = write(fd, data, size)) < 0)
            return r;
        if (r == 0)
            break;
        ret += r;
        data = (const uint8_t*) data + r;
        size -= (size_t) r;
    }
    return ret;
}

int main(int argc, char *argv[]) {
    struct addrinfo hints;
    struct addrinfo *result,*rp;
    char message[MAX_DATA_SIZE];
    char buffer[MAX_DATA_SIZE];
    ssize_t num_bytes;
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };
    pa_simple *s = NULL;
    int ret = 1;
    int error;
    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }
    // to control number of arguments passed to program
    if(argc!=3) {
        printf("Usage: ./client <server_ip> <listening_port_of_server>\n");
        return 0;
    }
    // installing signal handler to handle SIGINT
    if(signal(SIGINT,my_signal_handler)==SIG_ERR) {
        printf("Error in handling the SIGINT signal\n");
        return 0;
    }
    // to fill the sizeof(struct addrinfo) bytes of memory area pointed by &hints by zero
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;
    if(getaddrinfo(argv[1],argv[2],&hints,&result)!=0) {
        printf("Error in getaddrinfo\n");
        return 0;
    }
    for(rp=result;rp!=NULL;rp=rp->ai_next) {
        sfd=socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
        if(sfd==-1) {
            printf("Error in getting file descriptor to a socket");
            continue; // try next address
        }
        if(connect(sfd,rp->ai_addr,rp->ai_addrlen)==0) {
            break;
        }
        close(sfd); // try next address
    }
    if(rp==NULL) {
        printf("Could not connect to server\n");
        return 0;
    }
    freeaddrinfo(result);
    if(!fork()) {
        // while(fgets(message,MAX_DATA_SIZE,stdin)!=NULL) {
        //     if(send(sfd,message,strlen(message),0)==-1) {
        //         printf("Error in sending message to server\n");
        //     }
        // }
        for (;;) {
            uint8_t buf[BUFSIZE];
            /* Record some data ... */
            if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
                fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
                goto finish;
            }
            /* And write it to STDOUT */
            if (loop_write(sfd, buf, sizeof(buf)) != sizeof(buf)) {
                fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
                goto finish;
            }
        }
        ret = 0;
        finish:
        if (s)
            pa_simple_free(s);
        return ret;
        exit(1);
    }
    else {
        // while((num_bytes=recv(sfd,buffer,MAX_DATA_SIZE-1,0))>0) {
        //     buffer[num_bytes]='\0';
        //     printf("Client: Message from Server: %s",buffer);
        // }
    }
    return 0;
}