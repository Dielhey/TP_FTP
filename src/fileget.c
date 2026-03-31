/*
 * get - transfer a file from its name
 */
#include "fileget.h"
#include "request.h"

void fileget(char * filename, int connfd, long off){
    size_t n;
    rio_t rio;
    response_t res;
    int fd = open(filename, O_RDONLY, 0);
    if (fd < 0) {
        res.return_code = errno;
        res.size_block = 0;
        Rio_writen(connfd, &res, sizeof(response_t));
    }else{
        res.return_code = 0;
        Rio_readinitb(&rio, fd);
        res.size_text = 0;
    
        while((n = Rio_readnb(&rio, res.text, BLOCKSIZE)) > 0) {
            res.size_text += n; 
        }
        Close(fd);
        fd = open(filename, O_RDONLY, 0);
        if(off > 0) lseek(fd,off,SEEK_SET);

        Rio_readinitb(&rio, fd);
        while((n = Rio_readnb(&rio, res.text, BLOCKSIZE)) > 0) {
            res.size_block = n;
            Rio_writen(connfd, &res.return_code, sizeof(int));
            Rio_writen(connfd, &res.size_block, sizeof(size_t));
            Rio_writen(connfd, &res.size_text, sizeof(size_t));
            Rio_writen(connfd, res.text, n);   
        }
        Close(fd);
    }
}