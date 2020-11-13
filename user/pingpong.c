#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int main(int argc, char *argv[])
{

    int p1[2], p2[2];
    char s[5];
    pipe(p1);
    pipe(p2);
    int pid_child = fork();
    int pid_parent;
    if (pid_child == 0)
    {
        pid_child = getpid();

        read(p1[0], s, 4);
        close(p1[0]);
        printf("%d: received %c%c%c%c\n", pid_child, s[0], s[1], s[2], s[3]);

        //printf("child%d:sent pong\n", pid_child);
        write(p2[1], "pong", 4);
        close(p2[1]);
        exit();
    }
    else
    {
        pid_parent = getpid();

        //printf("parent%d:sent ping\n", pid_parent);
        write(p1[1], "ping", 4);
        close(p1[1]);

        read(p2[0], s, 4);
        close(p2[0]);
        printf("%d: received %c%c%c%c\n", pid_parent, s[0], s[1], s[2], s[3]);
        exit();
    }
}