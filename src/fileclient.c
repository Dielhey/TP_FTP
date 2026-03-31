/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include "fileget.h"
#include "request.h"

#define FILE_TMP "download.tmp"

int clientfd;

void sigint_handler(int sig) {
    if(!clientfd) exit(0);
    request_t req;
    req.type = BYE;
    Rio_writen(clientfd, &req, sizeof(request_t));
    Close(clientfd);
    exit(0);
}

void resGet(request_t req,long taille, char * str_taille){
    response_t res;
    ssize_t i;
    int fd = -1;
    int fd_tmp;
    time_t debut = time(NULL);
    char success = 0;
    while((i = Rio_readn(clientfd, &res.return_code, sizeof(int))) > 0) { 
        success = 1; 
        sleep(1);

        i = Rio_readn(clientfd, &res.size_block, sizeof(size_t));
        if(i <= 0) break;
        
        Rio_readn(clientfd, &res.size_text, sizeof(size_t));
        if(res.size_block >= 0){
        
            int n = 0;
            while(n != res.size_block){
                i = Rio_readn(clientfd, res.text, res.size_block-n);
                if(i <= 0){
                    success = 0;
                    printf("An error has occured : ");
                    printf("Packages loss\n");
                    break;
                }
                n += i;
            }
            
            if(res.return_code == 0){
                if (fd < 0){
                    fd = Open(req.filename, O_WRONLY | O_CREAT , S_IRUSR | S_IWUSR);
                    lseek(fd,req.offset,SEEK_SET);
                }
            
                fd_tmp = Open(FILE_TMP,O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR);
    
                
                write(fd, res.text, res.size_block);
                taille += res.size_block;

                sprintf(str_taille,"%ld",taille);
                write(fd_tmp,req.filename,strlen(req.filename));
                write(fd_tmp,"\n",1);
                write(fd_tmp,str_taille,strlen(str_taille));

                printf("%ld bytes received, %ld remains\n", res.size_block, (res.size_text - taille));
            } else {
                success = 0;
                printf("An error has occured : ");
                printf("%s\n", strerror(res.return_code));
                fflush(stdout);
            }
        }
        if(res.size_block < BLOCKSIZE){
            break;
        }
        
    }
    //usleep(1230000);


    time_t fin = time(NULL);
    
    if(success == 1){
        printf("Transfer successfully complete.\n");
        remove(FILE_TMP);
    }

    double temps = difftime(fin,debut);
        
    if(fd >= 0) Close(fd);

    if (temps == 0) {
    printf("%ld bytes received in %.2f seconds \n", taille, temps);

    } else {
        printf("%ld bytes received in %.2f seconds (%.2f Kbytes/s)\n", taille, temps,(float)(taille) / 1000.0 / temps);
    }
    fflush(stdout);  
}

void resLs(){
    char buf[MAXLINE];
    ssize_t n;

            while ((n = read(clientfd, buf, sizeof(buf)-1)) > 0) {
                buf[n] = '\0';
                printf("%s", buf);

                if (n < sizeof(buf)-1)
                    break;
            }
}

void resRm(request_t req){
    int i;
    response_t res;
    i = Rio_readn(clientfd, &res, sizeof(response_t));
    if(i>0 && res.return_code == 0){
        printf("File \"%s\" succesfully removed\n",req.filename);
    }else{
        printf("An error has occured : ");
        printf("%s\n", strerror(res.return_code));
    }
}

int main(int argc, char **argv)
{
    int port;
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
    Signal(SIGINT, sigint_handler);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    Rio_readinitb(&rio, clientfd);
    printf("client connected to server OS\n");
    int slave_port;
    int n;
    while((n = Rio_readn(clientfd,&slave_port,sizeof(int))) == 0){
        printf("%d\n",n);
        continue;
    }
    Close(clientfd);
    printf("Client connected to server %d\n",slave_port);
    clientfd = Open_clientfd(host, slave_port);


    while(1) {

        request_t req;
        long taille = 0;
        char str_taille[20];

        if(access(FILE_TMP, F_OK) == 0){
            FILE * f = fopen(FILE_TMP,"r");
            fgets(req.filename,MAXLINE,f);
            req.filename[strlen(req.filename)-1] = '\0';
            fgets(str_taille,20,f);
            taille = atol(str_taille);            
            req.offset = taille;
            req.type = GET;
            fclose(f);
            printf("Starting recuperation of %s \n",req.filename);
            fflush(stdout);

        }else{
            printf("ftp> ");
            fflush(stdout);
            Fgets(buf, MAXLINE, stdin);
            if (!strcmp(buf, "\n")){
                printf("Commande inconnue\n");
                continue;
            }
            // creation de la requete
            char* tok= strtok(buf, " \n");
            if(!strcmp(tok, "bye")) {
                Kill(getpid(), SIGINT);
            } else if(!strcmp(tok, "get")) {
                req.type = GET;
                tok = strtok(NULL, " \n");
                strcpy(req.filename,tok);
            } else if(!strcmp(tok, "ls")) {
                req.type = LS;
            } else if(!strcmp(tok, "put")) {
                req.type = PUT;
                tok = strtok(NULL, " \n");
                strcpy(req.filename,tok);
            } else if(!strcmp(tok, "rm")) {
                req.type = RM;
                tok = strtok(NULL, " \n");
                strcpy(req.filename,tok);
            } else {
                printf("Commande inconnue\n");
                continue;
            }
            req.offset = 0;
        }

        Rio_writen(clientfd, &req, sizeof(request_t));
        
        if(req.type == LS){
            resLs();
        }
        else if(req.type == GET){
            resGet(req,req.offset,str_taille);     
        }else if(req.type == RM){
            resRm(req);
        }else if(req.type == PUT){

        }
    }
    Close(clientfd);
}

