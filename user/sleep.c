#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("error input,you should input at least one.\n");
        exit();
    }
    if (argc > 2)
    {
        printf("error input,you should input at most one.\n");
        exit();
    }
    sleep(atoi(argv[1]));
    exit();
}