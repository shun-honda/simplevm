#include "input_source.h"
#include "loader.h"
#include <assert.h>
#include <string.h>

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

int main(int argc, char const *argv[])
{
    char pathbuf[4096] = {};
    InputSource o;
    memcpy(pathbuf, __FILE__, strlen(__FILE__));
    remove_filename(pathbuf);
    append_path(pathbuf, 4096, "byteCode.bin", strlen("byteCode.bin"));
    assert(InputSource_Init(&o, pathbuf) != NULL);
    assert(o.length > 0);
    assert(ByteCodeLoader_Load(&o) != NULL);
    InputSource_Dispose(&o);
    return 0;
}
