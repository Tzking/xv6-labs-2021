#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(const char *path, const char *name) {
    int fd;
    struct stat st;
    struct dirent de;
    char buf[512], *p;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        fprintf(2, "find: %s is not a directory\n", path);
        close(fd);
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof de) == sizeof de) {
        if (de.inum == 0) {
            continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0; // null terminate
        if (stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;
        }
        if (st.type == T_DIR && strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
            // 递归查找
            find(buf, name);
        } else if (strcmp(de.name, name) == 0) {
            printf("%s\n", buf);
        }

    }
    close(fd);
}
int main(int argc, char const *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <dir> <file>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
