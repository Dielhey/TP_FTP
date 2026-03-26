/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include "request.h"

int main(int argc, char **argv)
{
    int clientfd, port;
    char *host, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd(host, port);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n"); 
    
    Rio_readinitb(&rio, clientfd);

    Fgets(buf, MAXLINE, stdin);
    int n = strlen(buf);
    buf[n - 1] = '\n';
    Rio_writen(clientfd, buf, strlen(buf));
    ssize_t i;
    request_t req;
    char * tok = strtok(buf, " ");
    req.type = atoi(tok);
    tok = strtok(NULL, " ");
    req.filename = tok;

    int temp_out = dup(1);
    while((i = Rio_readlineb(&rio, buf, MAXLINE)) > 0) {    
        int fd = Open(req.filename, O_WRONLY | O_CREAT | O_TRUNC, 0);
        switch (req.type) {
        case GET:
            dup2(fd, 1);
            Fputs(buf, stdout);
            break;
        case LS:
            
            break;
        case PUT:
            
            break;    
        default:
            break;
        }
        
    };
    dup2(temp_out, 1);
    Close(clientfd);
    exit(0);
}
