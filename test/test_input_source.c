#include "input_source.h"
#include <assert.h>

int main(int argc, char const *argv[])
{
    InputSource o;
    InputSource_Init(&o, __FILE__);
    assert(o.length > 0);
    InputSource_Dispose(&o);
    return 0;
}

