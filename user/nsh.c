#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define MAXARGS 10
#define MAXBUF 100

int getcmd(char *buf, int nbuf)
{
    fprintf(1, "@ ");
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if (buf[0] == 0) // EOF
        return -1;
    return 0;
}

void handle_input_to_argv(char buf[], int *argc, char *argv[])
{

    // Read input commands.
    //buf末尾有一个\n,有一个结束0
    for (int j = 0; j < MAXARGS; j++)
    {
        argv[j] = 0;
    }
    int args_count = 0;
    int i = 0;
    while (buf[i++] == ' ')
        ;
    if (buf[i] != 0)
        argv[args_count++] = buf + i - 1;
    for (; i < MAXBUF; i++)
    {
        if (buf[i] == ' ')
        {
            buf[i] = '\0';
            while (buf[i++] == ' ')
                ;
            if (buf[i] != '\n' && buf[i] != ' ' && buf[i] != 0)
                argv[args_count++] = buf + i;
        }
        else if (buf[i] == '\n')
            buf[i] = '\0';
    }
    *argc = args_count;
}

void parse_argv(int argc, char *argv[])
{ /*
    fprintf(2, "para count=%d\n", argc);
    fprintf(2, "para list:\n");
    for (int i = 0; i < argc; i++)
    {
        fprintf(2, "   -%s___\n", argv[i]);
    }*/
    for (int i = 0; i < argc; i++)
    {
        if (argv[i][0] == '|')
        {
            argv[i] = 0;
            int p[2];
            pipe(p);
            int pid1, pid2;
            if ((pid1 = fork()) == 0)
            {

                close(1);
                dup(p[1]);
                close(p[0]);
                close(p[1]);
                parse_argv(i, argv);
            }
            if ((pid2 = fork()) == 0)
            {
                close(0);
                dup(p[0]);
                close(p[0]);
                close(p[1]);
                parse_argv(argc - i - 1, argv + i + 1);
            }
            wait(0);
            exit(0);
        }
    }
    for (int i = 0; i < argc; i++)
    {
        if (argv[i][0] == '>' || argv[i][0] == '<')
        {
            if (argv[i][0] == '>')
            {
                close(1);
                open(argv[i + 1], O_CREATE | O_WRONLY);
                argv[i] = 0;
            }
            else
            {
                close(0);
                open(argv[i + 1], O_CREATE | O_RDONLY);
                argv[i] = 0;
            }
        }
    }
    exec(argv[0], argv);
}

int main(int argc, char *argv[])
{
    static char buf[MAXBUF];
    int fd;
    char *exe_argv[MAXARGS];
    int exe_argc = 0;
    // Ensure that three file descriptors are open.
    while ((fd = open("console", O_RDWR)) >= 0)
    {
        if (fd >= 3)
        {
            close(fd);
            break;
        }
    }
    while (getcmd(buf, MAXBUF) >= 0)
    {
        if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ')
        {
            // Chdir must be called by the parent, not the child.
            buf[strlen(buf) - 1] = 0; // chop \n
            if (chdir(buf + 3) < 0)
                fprintf(1, "cannot cd %s\n", buf + 3);
            else
                fprintf(1, "have cd %s\n", buf + 3);
            continue;
        }
        handle_input_to_argv(buf, &exe_argc, exe_argv);
        if (exe_argc <= 0)
            continue;
        if (fork() == 0)
        {
            parse_argv(exe_argc, exe_argv);
            /*
            fprintf(1, "--------------\n");
            fprintf(1, "pid=%d run %s\n", getpid(), exe_argv[0]);
            fprintf(1, "para count=%d\n", exe_argc);
            fprintf(1, "para list:\n");
            for (int i = 0; i < exe_argc; i++)
            {
                fprintf(1, "   -%s___\n", exe_argv[i]);
            }
            fprintf(1, "--------------\n");
            */
        }
        wait(0);
    }
    exit(0);
}
