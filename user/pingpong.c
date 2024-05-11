#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char const *argv[]) {
    int p[2];
    char text[10];
    if (pipe(p) < 0) {
        printf("pipe error\n");
    }

    if (fork() == 0) {
        read(p[0], text, 10);
        printf("%d, received %s\n", getpid(), text);
        write(p[1], "pong", 10);
        exit(0);
    } else {
        write(p[1], "ping", 10);
        wait(0); // 等待子进程结束
        read(p[0], text, 10);
        printf("%d, received %s\n", getpid(), text);
        exit(0);
    }
    return 0;
}