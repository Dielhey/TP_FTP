/*
 * fileserveri.c - An iterative file server
 */
#include "csapp.h"
#include "request.h"
#include "fileget.h"

#define MAX_NAME_LEN 256
#define NB_PROC 10
#define PORT 2121

int pids[NB_PROC];
int is_finished = 1;

void sigchld_handler(int sig) {
    pid_t pid;
    while((pid = waitpid(-getpid(), NULL, 0)) > 0) {
        printf("Closed child %d with signal %d\n", pid, sig);
    }
}

void sigint_handler(int sig) {
    is_finished = 0;
    for(int i = 0; i < NB_PROC; i++) {
        Kill(pids[i], SIGINT);
    }
    exit(0);
}

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    Signal(SIGCHLD, sigchld_handler);
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(PORT);
    pid_t pid;
    for(int i = 0; i < NB_PROC; i++) {
        if((pid = Fork()) == 0) {
            break;
        }else {
            pids[i] = pid;
        }
    }
    if(pid > 0) {
        Signal(SIGINT, sigint_handler);
        while (1)
        {
            continue;
        }
    }
    request_t req;
    
    while (1) {
        if(!connfd) {
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            /* determine the name of the client */
            Getnameinfo((SA *) &clientaddr, clientlen,
                        client_hostname, MAX_NAME_LEN, 0, 0, 0);
            
            /* determine the textual representation of the client's IP address */
            Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                    INET_ADDRSTRLEN);
            
            printf("server connected to %s (%s)\n", client_hostname,
                client_ip_string);
        }
        
        rio_t rio;

        Rio_readinitb(&rio, connfd);
        ssize_t req_i = Rio_readn(connfd, &req, sizeof(request_t));
        if(req_i == 0) {
            printf("Connection closed unexpectedly with file %s (%s)\n", client_hostname,
                    client_ip_string);
            connfd = 0;
            continue;
        }
        switch (req.type) {
            case BYE:
                printf("Connection closed with file %s (%s)\n", client_hostname,
                    client_ip_string);
                Close(connfd);
                connfd = 0;
                break;
            case GET:
                printf("get file %s (%s)\n", client_hostname,
                    client_ip_string);
                fileget(req.filename, connfd, req.offset);
                break;
            case LS:
                break;
            case PUT:
                break;    
            default:
                break;
        }

    }
    Close(connfd);
    exit(0);
}

