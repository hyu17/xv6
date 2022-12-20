#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char* path, char* file_name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        printf("DEBUG: error: find can not open %s\n", path);
        exit(1);
    }

    if (fstat(fd, &st) < 0)
    {
        printf("DEBUG: error: find can not use stat %s\n", path);
        close(fd);
        exit(1);
    }
    if (st.type != T_DIR)
    {
        printf("DEBUG: error: the path should be a directory\n");
        close(fd);
        exit(1);
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
    {
        printf("DEBUG: error: find: path too long\n");
        exit(1);
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        // Avoid recursing "." and "..".
        if (de.inum == 0)
        {
            continue;
        }
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        {
            continue;
        }

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        printf("DEBUG: info: find: buf is %s, p is %s\n", buf, p);
        if (stat(buf, &st) < 0)
        {
            printf("DEBUG: error: find: cannot stat %s\n", p);
            continue;
        }
        if (st.type == T_DIR)
        {
            // printf("DEBUG: info: find: this dir is %s\n", buf);
            find(buf, file_name);
        }

        if (strcmp(p, file_name) == 0)
        {
            // Print the target path.
            // printf("DEBUG: info: find: the target path is %s\n", buf);
            printf("%s\n", buf);
        }
    }

    close(fd);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("DEBUG: error: type three arguments\n");
        exit(1);
    }

    char* path = argv[1];
    char* file_name = argv[2];
    printf("DEBUG: info: find: path is %s, file_name is %s\n",
           path, file_name);
    find(path, file_name);

    exit(0);
}