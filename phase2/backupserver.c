/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"
#include "stockdb.h"

#define CONNINFOMAX 1024
#define FDLISTMAX 64

typedef struct conn_info {
    int connfd;
    char client_hostname[CONNINFOMAX];
    char client_port[CONNINFOMAX];
} conn_info_t;


void echo(int connfd);
void readcommand(int);

void* server_thread(void *vargp);

int main(int argc, char **argv) 
{
    int listenfd; // connfd;
    conn_info_t* conn_info;
    pthread_t tid;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    // TODO: load stock.txt
    stockdb_load(); // test

    conn_info_t* conn_info_arr = Malloc(sizeof(conn_info_t) * FDLISTMAX);
    for (int i = 0; i < FDLISTMAX; i++) {
        conn_info_arr[i]->connfd = Open_listenfd_nonblocking(argv[1]);
    }

    for (int i = 0; i < FDLISTMAX; i++) {
        
        conn_info_arr[i]->connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        if (conn_info_arr[i]->connfd == -1) {
            continue;
        }
        else {
            
        }
    }


    while (1) {
        printf("listening\n");
        clientlen = sizeof(struct sockaddr_storage);
        conn_info = Malloc(sizeof(conn_info_t));
        conn_info->connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, conn_info->client_hostname, CONNINFOMAX, conn_info->client_port, CONNINFOMAX, 0);
        Pthread_create(&tid, NULL, server_thread, conn_info);
    }
    exit(0);
}
/* $end echoserverimain */

void show(char** buf_next) {
    stockdb_show(buf_next); 
}

void buy(char** buf_next, int id, int cnt) {
    switch (stockdb_buy(id, cnt)) {
        case STOCKDB_SUCCESS:
            sprintf(*buf_next, "[buy] success\n");
            break;
        case STOCKDB_FAILED:
            sprintf(*buf_next, "Not enough left stock\n");
            break;
        case STOCKDB_ERROR:
            sprintf(*buf_next, "INVALID STOCK ID\n");
            break;
    }
}

void sell(char** buf_next, int id, int cnt) {
    if (stockdb_sell(id, cnt) == STOCKDB_SUCCESS) {
        sprintf(*buf_next, "[sell] success\n");
    }
    else {
        sprintf(*buf_next, "INVALID STOCK ID\n");
    }
}


void readcommand(int connfd) 
{
    int n; 
    char buf[MAXLINE];
    char* buf_next = buf;
    rio_t rio;
    char command[MAXLINE];
    int id, cnt;
    int msglen;
    char msglen_text[16];
    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        buf_next = buf;
        printf("server received %d bytes\n", n);
        sscanf(buf, "%s %d %d", command, &id, &cnt);
        printf("command = %s\n", command);
        if (strcmp("show", command) == 0) {
            show(&buf_next);
        }
        else if (strcmp("buy", command) == 0) {
            buy(&buf_next, id, cnt);
        }
        else if (strcmp("sell", command) == 0) {
            sell(&buf_next, id, cnt);
        }
        else if (strcmp("exit", command) == 0) {
            msglen = -1;
            sprintf(msglen_text, "%d\n", msglen);
            printf("msglen_text: %s\n", msglen_text);
            Rio_writen(connfd, msglen_text, strlen(msglen_text));
            break;
        }
        else {

        }
        msglen = strlen(buf);
        sprintf(msglen_text, "%d\n", msglen);
        printf("msglen sent: %d\n", msglen);
        printf("msglen_text: %s\n", msglen_text);
        Rio_writen(connfd, msglen_text, strlen(msglen_text));
        Rio_writen(connfd, buf, msglen);
        puts("server write end, content sent: ");
        puts(buf);
        puts("---");
    }
}

void* server_thread(void* vargp) {
    conn_info_t conn_info;
    memcpy(&conn_info, (conn_info_t*)vargp, sizeof(conn_info_t));
    Pthread_detach(pthread_self());
    Free(vargp);
    readcommand(conn_info.connfd);
    printf("Connection Closed (%s %s)\n", conn_info.client_hostname, conn_info.client_port);
    Close(conn_info.connfd);
    return NULL;
}

/* $end echo */

void echo(int connfd) 
{
    int n; 
    char buf[MAXLINE]; 
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("server received %d bytes\n", n);
        Rio_writen(connfd, buf, n);
    }
}
