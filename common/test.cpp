#include "basis.h"
#include <iostream>

int main() {
    // Create a sample data structure
    rpcmem_t *mem = rpcmem_new();
    string originalString = "Hello, World!";
    int originalInt = 42;
    float originalFloat = 3.14;
    bool originalBool = true;

    // Pack the data
    pack_int(originalInt, mem);
    pack_float(originalFloat, mem);
    pack_string(originalString, mem);
    pack_bool(originalBool, mem);

    // Display the packed data (for demonstration purposes)
    // cout << "Packed Data: ";
    // for (rpcptr_t i = mem->hp; i < mem->capacity; ++i) {
    //     cout << static_cast<int>(mem->data[i]) << " ";
    // }
    // cout << endl;

    // Unpack the data
    bool unpackedBool = unpack_bool(mem);
    string unpackedString = unpack_string(mem);
    float unpackedFloat = unpack_float(mem);
    int unpackedInt = unpack_int(mem);

    // Display the unpacked data
    cout << "Unpacked String: " << unpackedString << endl;
    cout << "Unpacked Int: " << unpackedInt << endl;
    cout << "Unpacked Float: " << unpackedFloat << endl;
    cout << "Unpacked Bool: " << unpackedBool << endl;

    // Clean up
    rpcmem_free(&mem);

    return 0;
}
