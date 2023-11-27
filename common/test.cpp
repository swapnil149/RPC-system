#include "basis.h"
#include <iostream>

int main() {
    // Create a sample data structure
    __rpcmem_t *mem = rpcmem_new();
    string originalString = "Hello, World!";
    int originalInt = 0x12345678;
    float originalFloat = 3.14;
    bool originalBool = true;

    string ss[3] = {"Hey", "hello", "hi"};

    // Pack the data
    __pack_int(originalInt, mem);
    __pack_float(originalFloat, mem);
    __pack_string(originalString, mem);
    __pack_bool(originalBool, mem);

    for (int i = 0; i < 3; i++)
        __pack_string(ss[i], mem);

    // compress the data
    DEBUG("%d", mem->capacity - mem->sp);
    DEBUG("%d", mem->capacity);
    DEBUG("%d", mem->sp);
    DEBUG("%d", originalInt);

    // DEBUGBUF(mem->data, mem->sp, mem->capacity);
    int mem_size = mem->hp + (mem->capacity - mem->sp);
    memmove(mem->data + mem->hp, mem->data + mem->sp, mem->capacity - mem->sp);

    mem->sp = mem->hp;
    // DEBUGBUF(mem->data, 0, mem_size);

    // Unpack the data
    string unpackedss[3];
    for (int i = 3; i > 0; i--)
        unpackedss[i - 1] = __unpack_string(mem);

    bool unpackedBool = __unpack_bool(mem);
    string unpackedString = __unpack_string(mem);
    float unpackedFloat = __unpack_float(mem);
    int unpackedInt = __unpack_int(mem);

    // // Display the unpacked data
    cout << "Unpacked String[0]: " << unpackedss[0] << endl;
    cout << "Unpacked String[1]: " << unpackedss[1] << endl;
    cout << "Unpacked String: " << unpackedString << endl;
    cout << "Unpacked Float: " << unpackedFloat << endl;
    cout << "Unpacked Bool: " << unpackedBool << endl;
    cout << "Unpacked Int: " << unpackedInt << endl;

    // Clean up
    rpcmem_free(&mem);

    return 0;
}
