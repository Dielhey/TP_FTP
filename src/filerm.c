/*
 * cat - read a file from its name
 */
#include "filerm.h"



void filerm(request_t req,int connfd){

    response_t res;

    if (remove(req.filename) == 0) {
        res.return_code = 0;
        res.size_block = 0;
        Rio_writen(connfd, &res, sizeof(response_t));
    }else{
        res.return_code = errno;
        res.size_block = 0;
        Rio_writen(connfd, &res, sizeof(response_t));
    }

}
