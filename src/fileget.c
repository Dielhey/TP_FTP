/*
 * cat - read a file from its name
 */
#include "fileget.h"

#define GET 0
#define PUT 1
#define LS 2

response_t fileget(char * filename, int connfd){
    response_t res;
    size_t n;
    rio_t rio;
    int fd = open(filename, O_RDONLY, 0);
    if (fd < 0) {
        res.return_code = errno;
        return res;
    }
    Rio_readinitb(&rio, fd);
    while((n = Rio_readnb(&rio, res.text, 1)) > 0) {
        res.text += n;
    }
    res.return_code = 0;
    return res;
}
