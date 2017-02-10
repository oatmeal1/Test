#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>

#include "wrap.h"

#define MAXLINE 8192
#define SERV_PORT 2500


#define CREATDAEMON \
    if (fork() > 0) exit(0);\
    setsid();

void do_sigchild(int num)
{
    while (waitpid(0, NULL, WNOHANG) > 0)
        ;
}

int main(void)
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int listenfd, connfd;
    pid_t pid;
    struct sigaction newact;

    CREATDAEMON

    newact.sa_handler = do_sigchild;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGCHLD, &newact, NULL);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, 20);

    printf("Accepting connections ...\n");

    while (1) {

        cliaddr_len = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
        printf("-------------------------%d\n", connfd);

        pid = fork();
        if (pid == 0) {
            Close(listenfd);

            dup2(connfd, STDOUT_FILENO);
            dup2(connfd, STDIN_FILENO);
            execl("/home/itheima/xhttpd", "xhttpd", "/home/itheima/dir", NULL);  //./xhttpd /home/itcat/dir
            
        } else if (pid > 0) {
            Close(connfd);
        } else
            perr_exit("fork");
    }
    return 0;
}


