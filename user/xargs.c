#include "user/user.h"
#include "kernel/param.h"
#include "kernel/stat.h"

int main(int argc, char *argv[])
{
    char *args[MAXARG];
    for (int i = 1; i < argc; i++)
    {
        args[i - 1] = argv[i];
    }
    args[argc - 1] = malloc(512);
    args[argc] = 0;
    while (gets(args[argc - 1], 512)) {
        if (args[argc - 1][0] == 0) break;
        if (args[argc - 1][strlen(args[argc - 1]) - 1] == '\n') {
            args[argc - 1][strlen(args[argc - 1]) - 1] = 0;
        }
        if (fork() == 0) {
            exec(args[0], args);
        } else {
            wait(0);
        }
    }
    exit(0);
}
