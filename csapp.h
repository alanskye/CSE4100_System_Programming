#ifndef __CSAPP_H__
#define __CSAPP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>

typedef void handler_t(int);

ssize_t sio_puts(char s[]);
ssize_t sio_putl(long v);
void sio_error(char s[]);

ssize_t Sio_puts(char s[]);
ssize_t Sio_putl(long v);
void Sio_error(char s[]);

void unix_error(char* msg);

pid_t Fork();


handler_t* Signal(int signum, handler_t* handler);

unsigned int Sleep(unsigned int secs);

#endif
