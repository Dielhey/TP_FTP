typedef enum typereq_t {
    GET = 0,
    PUT = 1,
    LS = 2
}typereq_t;

typedef struct request_t {
    typereq_t type;
    char * filename;
}request_t;

typedef struct response_t {
    int return_code;
    char * text;
}response_t;