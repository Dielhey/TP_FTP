#include "csapp.h"
#include "fileget.h"

#define NB_SLAVES 2
#define PORT 2121
#define BASE_SLAVE_PORT 3000

int main() {

    int slavefds[NB_SLAVES];
    int slave_ports[NB_SLAVES];

    for (int i = 0; i < NB_SLAVES; i++) {
        slave_ports[i] = BASE_SLAVE_PORT + i + 1;

        char port_str[16];
        sprintf(port_str, "%d", slave_ports[i]);

        slavefds[i] = Open_clientfd("127.0.0.1", slave_ports[i]);

        if (slavefds[i] < 0) {
            fprintf(stderr, "Can't connect to slave %d\n", i);
            exit(1);
        }

        printf("Connected to slave %d on port %d\n", i, slave_ports[i]);
    }

    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    int current_slave = 0;
    rio_t rio;


    clientlen = sizeof(clientaddr);
    listenfd = Open_listenfd(PORT);
    printf("Master listening on port %d\n", PORT);
        
    while (1) {
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Rio_readinitb(&rio, connfd);


        int slave = current_slave;

        printf("Client assigned to slave %d (port %d)\n",
            slave, slave_ports[slave]);

        Rio_writen(connfd,&slave_ports[current_slave], sizeof(int));
        printf("%d\n",slave_ports[current_slave]);

        current_slave = (current_slave + 1) % NB_SLAVES;
        Close(connfd);
    }
}
