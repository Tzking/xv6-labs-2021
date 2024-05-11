#include "kernel/types.h"

struct stat;
struct rtcdate;

// system calls
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int *);
int pipe(int *);

/**
 * @brief 写文件
 * @param fd 文件描述符
 * @param buf 缓冲区
 * @param size 写入的字节数
 * @return 0 成功, -1 失败
 */
int write(int fd, const void *buf, int size);

/**
 * @brief 读取文件
 * @param fd 文件描述符
 * @param buf 缓冲区
 * @param size 读取的字节数
 * @return 0 成功, -1 失败
 */
int read(int fd, void *buf, int size);

int close(int);
int kill(int);
int exec(char *, char **);
int open(const char *, int);
int mknod(const char *, short, short);
int unlink(const char *);
int fstat(int fd, struct stat *);
int link(const char *, const char *);
int mkdir(const char *);
int chdir(const char *);
int dup(int);
int getpid(void);
char *sbrk(int);
int sleep(int);
int uptime(void);

// ulib.c
int stat(const char *, struct stat *);
char *strcpy(char *, const char *);
void *memmove(void *, const void *, int);
char *strchr(const char *, char c);
int strcmp(const char *, const char *);
void fprintf(int, const char *, ...);
void printf(const char *, ...);
char *gets(char *, int max);
uint strlen(const char *);
void *memset(void *, int, uint);
void *malloc(uint);
void free(void *);
int atoi(const char *);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
