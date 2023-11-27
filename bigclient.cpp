#include "big.idl"
#include "debugmacros.h"

int rpc_main(int argc, char **argv) {
    int thirtytwo_k_numbers[10][10][4][10][2];
    for (int i = 0; i < 10 * 10 * 10 * 4 * 2; i++) {
        thirtytwo_k_numbers[0][0][0][0][i] = i * i;
    }

    int n2000 = nth_number_32k(thirtytwo_k_numbers, 2000);
    DEBUG("%d", n2000);

    int eighty_k_numbers[10][10][10][10][2];
    for (int i = 0; i < 10 * 10 * 10 * 10 * 2; i++) {
        eighty_k_numbers[0][0][0][0][i] = i * i;
    }

    int n70000 = nth_number_80k(eighty_k_numbers, 70000);
    DEBUG("%d", n70000);

    return 0;
}
