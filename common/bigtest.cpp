#define INITIAL_CAPACITY 16
#include "../basis.h"
#include "../mem.h"
#include "debugmacros.h"

int main() {
    __rpcmem_t *mem = rpcmem_new();
    int x = 0xabcdef42;
    string hello = "hello there!";

    ERP("\nNO DATA\n");
    DEBUG("%d", mem->sp);
    DEBUG("%d", mem->hp);
    DEBUGBUF(mem->data, 0, mem->capacity);

    __pack_int(x, mem);

    ERP("\nONE INTEGER\n");
    DEBUG("%d", mem->sp);
    DEBUG("%d", mem->hp);
    DEBUGBUF(mem->data, 0, mem->capacity);

    __pack_string(hello, mem);

    ERP("\nA STRING\n");
    DEBUG("%d", mem->sp);
    DEBUG("%d", mem->hp);
    DEBUGBUF(mem->data, 0, mem->capacity);

    string hello_out = __unpack_string(mem);
    int x_out = __unpack_int(mem);

    DEBUG("0x%x", x_out);
    DEBUG("%s", hello_out.c_str());

    rpcmem_free(&mem);
    return 0;
}
