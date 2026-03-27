/*
 * cat - read a file from its name
 */
#include "fileget.h"
#include "request.h"

#define GET 0
#define PUT 1
#define LS 2

/*response_t fileget(char * filename, int connfd){
    response_t res;
    size_t n;
    rio_t rio;
    int fd = open(filename, O_RDONLY, 0);
    if (fd < 0) {
        res.return_code = errno;
        return res;
    }
    char text[MAXBUF];
    Rio_readinitb(&rio, fd);
    while((n = Rio_readnb(&rio, text, 1)) > 0) {
        Rio_writen(connfd, text, n);
        fprintf(stderr," Contenu : %s\n",res.text);
        Write(connfd,res.text,n);
        res.text += n;
    }
    res.return_code = 0;
    return res;
}*/

void fileget(char * filename, int connfd){
    size_t n;
    rio_t rio;
    response_t res;
    int fd = open(filename, O_RDONLY, 0);
    if (fd < 0) {
        res.return_code = errno;
        res.size_text = 0;
        Rio_writen(connfd, &res, sizeof(response_t));
    }else{
        res.return_code = 0;
        Rio_readinitb(&rio, fd);

        while((n = Rio_readnb(&rio, res.text, MAXLINE)) > 0) {
            res.size_text = n;
            Rio_writen(connfd, &res.return_code, sizeof(int));
            Rio_writen(connfd, &res.size_text, sizeof(size_t));
            Rio_writen(connfd, res.text, n);   
        }
        Close(fd);
    }

    
    
}
