#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

void input_and_exec(int argc, char *argv[])
{
    char buf[512];
    char t;
    int i = 0;
    while (read(0, &t, sizeof(char) == 1) && t != '\n')
    {
        buf[i++] = t;
    }
    if (t == '\n')
    {
        buf[i++] = '\0';
        char *new_arg[argc + 1];
        for (int i = 0; i < argc - 1; i++)
        {
            new_arg[i] = argv[i + 1];
        }
        new_arg[argc - 1] = buf;
        new_arg[argc] = '\0';

        //正序输出
        if (fork() == 0)
        {
            exec(new_arg[0], new_arg);
        }
        else
        {
            wait();
            input_and_exec(argc, argv);
        }
        /*
        //逆序输出
        if (fork() == 0)
        {
            input_and_exec(argc, argv);
        }
        else
        {
            wait();
            exec(new_arg[0], new_arg);
        }*/
    }
    exit();
}

int main(int argc, char *argv[])
{
    input_and_exec(argc, argv);
    exit();
}
