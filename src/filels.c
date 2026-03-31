/*
 * ls - list the files available
 */
#include "filels.h"
#include "request.h"

void filels(int connfd) {
    int pid_ls;
    if((pid_ls = Fork())== 0){
        dup2(connfd, STDOUT_FILENO);
        dup2(connfd, STDERR_FILENO);

        execlp("ls", "ls", "-l", NULL);

        perror("exec failed");
        
        exit(1);
    }else if (pid_ls > 0) {
        waitpid(pid_ls, NULL, 0);
    } 
    else {
        perror("fork failed");
    }
}