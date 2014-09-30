#include <string.h>
#ifndef TEST_UTIL_H
#define TEST_UTIL_H

static size_t find_parent_path_end(char *buf, size_t len)
{
    size_t end = len;
    while (end > 0 && buf[end - 1] != '/') {
        end--;
    }
    return end;
}

static void remove_filename(char *pathbuf)
{
    size_t pos = find_parent_path_end(pathbuf, strlen(pathbuf));
    if (pos > 0) {
        pathbuf[pos] = 0;
    }
}

static int append_path(char *buf, size_t bufsize, const char *path, size_t psize)
{
    size_t len = strlen(buf);
    if (len + psize + 1 >= bufsize) {
        return 1;
    }
    strncpy(buf + len, path, psize);
    buf[len + psize] = 0;
    return 0;
}

#endif /* end of include guard */
