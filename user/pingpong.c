#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    int fds[2];
    char buf_child[8];
    char buf_parent[8];
    int pid;

    pipe(fds);
    pid = fork();

    if (pid == 0)
    {
        printf("DEBUG: this is child\n");
        if (read(fds[0], buf_child, 8) == 0)
        {
            printf("DEBUG: error: read error\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());
        write(fds[1], buf_child, 8);
        close(fds[0]);
        close(fds[1]);
        exit(0);
    }
    else
    {
        printf("DEBUG: this is parent\n");
        if (write(fds[1], "7", 8) != 8)
        {
            printf("DEBUG: error: write error\n");
            exit(1);
        }
        wait(0);
        read(fds[0], buf_parent, 8);
        printf("%d: received pong\n", getpid());
        close(fds[0]);
        close(fds[1]);
        exit(0);
    }
    
    exit(0);
}