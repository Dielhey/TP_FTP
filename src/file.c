/*
 * cat - read a file from its name
 */
#include "csapp.h"
#include "request.h"

#define GET 0
#define PUT 1
#define LS 2

void file(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    n = Rio_readlineb(&rio, buf, MAXLINE);
    request_t req;
    char * tok = strtok(buf, " ");
    req.type = tok;
    tok = strtok(NULL, " ");
    req.filename = tok;
    printf("server received %u bytes\n", (unsigned int)n);
    Rio_writen(connfd, buf, n);
}
