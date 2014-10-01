#include "input_source.h"
#include "loader.h"
#include "test_util.h"
#include <assert.h>
#include <string.h>

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
