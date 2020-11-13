#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void received(int p[2])
{
    close(p[1]);
    int first_element, element, pid;
    int byte_count = read(p[0], &first_element, sizeof(int));
    pid = getpid();
    if (byte_count > 0)
        printf("prime %d\n", first_element);
    else
    {
        exit();
    }
    int q[2];
    pipe(q);
    pid = fork();
    if (pid != 0)
    {
        // parent
        close(q[0]);
        while (read(p[0], &element, sizeof(int)))
        {
            if (element % first_element != 0)
            {
                write(q[1], &element, sizeof(int));
            }
        }
        close(p[0]);
        close(q[1]);
        wait();
        exit();
    }
    else
    {
        // child
        received(q);
    }
}

int main(int argc, char *argv[])
{
    int p[2];
    pipe(p);

    int pid = fork();
    if (pid != 0)
    {
        // parent
        close(p[0]);
        for (int i = 2; i < 36; i++)
        {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);
    }
    else
    {
        // child
        received(p);
    }
    wait();
    exit();
}
