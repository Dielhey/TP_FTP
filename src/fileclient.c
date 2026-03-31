/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include "fileget.h"
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
    printf("ftp> ");
    
    Rio_readinitb(&rio, clientfd);

    Fgets(buf, MAXLINE, stdin);

    // creation de la requete
    request_t req;
    char* tok= strtok(buf, " \n");
    req.type = atoi(tok);
    tok = strtok(NULL, " \n");
    strcpy(req.filename,tok);

    Rio_writen(clientfd, &req, sizeof(request_t));

    response_t res;
    ssize_t i;
    int fd = -1;
    long taille = 0;
    time_t debut = time(NULL);
    char success = 1;
    while((i = Rio_readn(clientfd, &res.return_code, sizeof(int))) > 0) {  
        if(i <= 0) break;

        i = Rio_readn(clientfd, &res.size_text, sizeof(size_t));
        if(i <= 0) break;

        if(res.size_text >= 0){
        
            int n = 0;
            while(n != res.size_text){
                i = Rio_readn(clientfd, res.text, res.size_text-n);
                if(i <= 0){
                    success = 0;
                    printf("An error has occured : ");
                    printf("Packages loss\n");
                    break;
                }
                n += i;
            }
            
            if(res.return_code == 0){
                if (fd <0) fd = Open(req.filename, O_WRONLY | O_CREAT , S_IRUSR | S_IWUSR);
                write(fd, res.text, res.size_text);
                taille += res.size_text;
            } else {
                success = 0;
                printf("An error has occured : ");
                printf("%s\n", strerror(res.return_code));
                fflush(stdout);
            }
        }
        if(res.size_text < MAXBUF){
            break;
        }
        
    }
    //usleep(1230000);

   
    time_t fin = time(NULL);
    
    if(success == 1){
        printf("Transfer successfully complete.\n");
    }

    double temps = difftime(fin,debut);
        
    if(fd >= 0) Close(fd);

    if (temps == 0) {
    printf("%ld bytes received in %.2f seconds \n", taille, temps);

    } else {
        printf("%ld bytes received in %.2f seconds (%.2f Kbytes/s)\n", taille, temps,(float)(taille) / 1000.0 / temps);
    }
    fflush(stdout);
    Close(clientfd);
    exit(0);
}