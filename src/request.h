#include "csapp.h"

typedef enum typereq_t {
    BYE = -1,
    GET = 0,
    PUT = 1,
    RM = 2,
    LS = 3
}typereq_t;

typedef struct request_t {
    typereq_t type;
    char filename[MAXLINE];
    long offset;
}request_t;

typedef struct response_t {
    int return_code;
    size_t size_block;
    size_t size_text;
    char text[BLOCKSIZE];
    
}response_t;