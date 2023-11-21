#include "basis.h"
#include <iostream>

//                 "<fname> <stackptr & heapptr>"
#define PREFIX_FMT "%s %08x"

#define MAX_FNAME_LEN 80
#define MAX_PREFIX_LEN (MAX_FNAME_LEN + 20) // rough estimate

#undef PREFIX_FMT
#undef MAX_PREFIX_LEN

int main() {
    // Create a sample data structure
    __rpcmem_t *mem = rpcmem_new();
    string originalString = "Hello, World!";
    int originalInt = 42;
    float originalFloat = 3.14;
    bool originalBool = true;

    // Pack the data
    __pack_int(originalInt, mem);
    __pack_float(originalFloat, mem);
    __pack_string(originalString, mem);
    __pack_bool(originalBool, mem);

    // Display the packed data (for demonstration purposes)
    // cout << "Packed Data: ";
    // for (rpcptr_t i = mem->hp; i < mem->capacity; ++i) {
    //     cout << static_cast<int>(mem->data[i]) << " ";
    // }
    // cout << endl;

    // Unpack the data
    bool unpackedBool = __unpack_bool(mem);
    string unpackedString = __unpack_string(mem);
    float unpackedFloat = __unpack_float(mem);
    int unpackedInt = __unpack_int(mem);

    // Display the unpacked data
    cout << "Unpacked String: " << unpackedString << endl;
    cout << "Unpacked Int: " << unpackedInt << endl;
    cout << "Unpacked Float: " << unpackedFloat << endl;
    cout << "Unpacked Bool: " << unpackedBool << endl;

    // Clean up
    rpcmem_free(&mem);

    return 0;
}
