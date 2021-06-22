/*
 * echoclient.c - An echo client
 */
/* $begin echoclientmain */
#include "csapp.h"

int main(int argc, char **argv) 
{
    int clientfd;
    char *host, *port, buf[MAXLINE];
    int msglen;
    rio_t rio;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = argv[2];

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        Rio_writen(clientfd, buf, strlen(buf));
        // read exactly n bytes
        Rio_readlineb(&rio, buf, MAXLINE);
        printf("buffer content: %s\n", buf);
        msglen = -1;
        sscanf(buf, "%d", &msglen);
        printf("msg len expected: %d\n", msglen);
        if (msglen == -1) {
            break;
        }
        Rio_readnb(&rio, buf, msglen);
        Fputs(buf, stdout);
    }
    Close(clientfd); //line:netp:echoclient:close
    exit(0);
}
/* $end echoclientmain */
