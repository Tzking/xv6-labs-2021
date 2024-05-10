#include "user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: sleep ticks\n");
        exit(1);
    }
    printf("Sleeping for %d ticks\n", atoi(argv[1]));
    sleep(atoi(argv[1]));
    printf("Woke up after %d ticks\n", atoi(argv[1]));
    exit(0);
}