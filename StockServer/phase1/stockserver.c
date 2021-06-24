/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"
#include "stockdb.h"

#define CONNINFOMAX 1024

typedef struct {
    int maxfd; // maximum fd in read set
    fd_set read_set;
    fd_set ready_set;
    int cnt_ready; // number of ready fds
    int maxi; 
    int clientfd[FD_SETSIZE]; // inactive fd == -1, else, active
    rio_t clientrio[FD_SETSIZE];
} pool;

void init_pool(int listenfd, pool* p);
void add_client(int connfd, pool* p);
void check_clients(pool* p);

void show(char** buf_next);
void sell(char** buf_next, int id, int cnt);
void buy(char** buf_next, int id, int cnt);

// signal handler for ctrl + z
void sigtstp_handler(int signum) {
    printf("\n=== storing stockdb.txt ===\n");
    stockdb_save();
    printf("\n=== server quit!        ===\n");
}

int main(int argc, char **argv) 
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    static pool pool;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    Signal(SIGTSTP, sigtstp_handler);
    printf("Event Based Stock Server\n");
    printf("Press Ctrl+Z to quit the server and store stock.txt\n");

    // TODO: load stock.txt
    stockdb_load(); // test

    listenfd = Open_listenfd(argv[1]);
    
    init_pool(listenfd, &pool);

    while (1) {
        pool.ready_set = pool.read_set;
        pool.cnt_ready = Select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);
        // non blocking connection accept
        if (FD_ISSET(listenfd, &pool.ready_set)) {
            clientlen = sizeof(struct sockaddr_storage);
            connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
            add_client(connfd, &pool);
        }
        // iterate over fd array
        check_clients(&pool);
    }

    exit(0);
}
/* $end echoserverimain */

void init_pool(int listenfd, pool* p) {
    // initialize client fd array
    p->maxi = -1;
    for (int i = 0; i < FD_SETSIZE; i++) {
        p->clientfd[i] = -1;
    }
    p->maxfd = listenfd;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set);
}

void add_client(int connfd, pool* p) {
    int i;
    p->cnt_ready--;
    for (i = 0; i < FD_SETSIZE; i++) {
        if (p->clientfd[i] < 0) {
            p->clientfd[i] = connfd;
            Rio_readinitb(&p->clientrio[i], connfd);
            FD_SET(connfd, &p->read_set);
            if (connfd > p->maxfd) {
                p->maxfd = connfd;
            }
            if (i > p->maxi) {
                p->maxi = i;
            }
            break;
        }
    }
    if (i == FD_SETSIZE) {
        puts("TOO MANY CLIENTS!");
    }
}

void check_clients(pool* p) {
    int i, connfd, n;
    char buf[MAXLINE];
    rio_t rio;
    char* buf_next = buf;
    char command[MAXLINE];
    int id, cnt;
    int msglen;
    char msglen_text[16];
    for (i = 0; (i <= p->maxi) && (p->cnt_ready > 0); i++) {

        connfd = p->clientfd[i];
        rio = p->clientrio[i];

        // there is pending request
        if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) {
            p->cnt_ready--;

            if ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
                buf_next = buf;
                sscanf(buf, "%s %d %d", command, &id, &cnt);
                // check user request
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
                // send message length
                msglen = strlen(buf);
                sprintf(msglen_text, "%d\n", msglen);
                Rio_writen(connfd, msglen_text, strlen(msglen_text));
                // send message (buffer)
                Rio_writen(connfd, buf, msglen); 
            }
            else { // EOF detected (connection tear down)
                Close(connfd);
                FD_CLR(connfd, &p->read_set);
                p->clientfd[i] = -1;
            }
        }
    }
}

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
            printf("error id: %d\n", id);
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
