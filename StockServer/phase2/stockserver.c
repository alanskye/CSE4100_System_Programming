/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"
#include "stockdb.h"

#define CONNINFOMAX 1024

typedef struct conn_info {
    int connfd;
    char client_hostname[CONNINFOMAX];
    char client_port[CONNINFOMAX];
} conn_info_t;


void readcommand(int);

void* server_thread(void *vargp);

void sigtstp_handler(int signum) {
    printf("\n=== storing stockdb.txt ===\n");
    stockdb_save();
    printf("\n=== server quit!        ===\n");
}

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

    Signal(SIGTSTP, sigtstp_handler);
    printf("Pthread Stock Server\n");
    printf("Press Ctrl+Z to quit the server and store stock.txt\n");

    // TODO: load stock.txt
    stockdb_load();

    listenfd = Open_listenfd(argv[1]);
    while (1) {
        // allocation for new thread
        clientlen = sizeof(struct sockaddr_storage);
        conn_info = Malloc(sizeof(conn_info_t));
        // blocking system call
        conn_info->connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, conn_info->client_hostname, CONNINFOMAX, conn_info->client_port, CONNINFOMAX, 0);
        // create a new thread for the client
        Pthread_create(&tid, NULL, server_thread, conn_info);
    }
    puts("end");
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

        sscanf(buf, "%s %d %d", command, &id, &cnt);

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
            
            Rio_writen(connfd, msglen_text, strlen(msglen_text));
            break;
        }
        msglen = strlen(buf);
        sprintf(msglen_text, "%d\n", msglen);
        Rio_writen(connfd, msglen_text, strlen(msglen_text));
        Rio_writen(connfd, buf, msglen);
    }
}

void* server_thread(void* vargp) {
    conn_info_t conn_info;
    memcpy(&conn_info, (conn_info_t*)vargp, sizeof(conn_info_t));
    // set automatic resource free
    Pthread_detach(pthread_self());
    Free(vargp);
    readcommand(conn_info.connfd);
    Close(conn_info.connfd);
    return NULL;
}
