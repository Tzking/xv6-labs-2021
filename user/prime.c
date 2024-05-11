#include "user/user.h"
#include "kernel/types.h"
#include "kernel/stat.h"

__attribute__((noreturn))
void primes(int p[2]) {
    close(p[1]); // close write end
    int first;
    int data;
    if (read(p[0], &first, sizeof(int)) == sizeof(int)) {
        printf("prime %d\n", first);
        int q[2];
        pipe(q);
        while (read(p[0], &data, sizeof(int)) == sizeof(int)) {
            if (data % first != 0) {
                write(q[1], &data, sizeof(int));
            }
        }
        close(p[0]);
        close(q[1]);
        if (fork() == 0) {
            primes(q);
        } else {
            close(q[0]);
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);
    for (int i = 2; i <= 35; i++)
    {
        write(p[1], &i, sizeof(int));
    }
    if (fork() == 0) {
        primes(p);
    } else {
        close(p[0]);
        close(p[1]);
        wait(0);
    }
    exit(0);
}