/*
 * cat - read a file from its name
 */
#include "csapp.h"
#include "fileget.h"

#define GET 0
#define PUT 1
#define LS 2

void fileget(char * filename, int connfd){
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
    printf("file : %s\n",filename);
    int fd = Open(filename, O_RDONLY | O_CREAT, 0);
    Rio_readinitb(&rio, fd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    // printf("server received %u bytes\n", (unsigned int)n);
        Rio_writen(connfd, buf, n);
    }
}
