/*
 * cat - read a file from its name
 */
#include "csapp.h"
#include "request.h"

#define GET 0
#define PUT 1
#define LS 2

void file(char * filename, int connfd){
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
    int fd = Open(filename, O_RDONLY, NULL);
    Rio_readinitb(&rio, fd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    // printf("server received %u bytes\n", (unsigned int)n);
        Rio_writen(connfd, buf, n);
    }
}
