#include <stdlib.h>
#include <string>
using namespace std;

#include "debugmacros.h"
#include "person.idl"

int rpc_main(int argc, char **argv) {
    (void)argc, (void)argv;

    MADEIT;

    int nums[8][8][8][8];
    People ppl;

    for (int i = 0; i < 8 * 8 * 8 * 8; i++)
        nums[0][0][0][i] = 20 + i;

    Person jacob = {"Jacob", 23, {1, 2, 3, 4}};
    ppl.people[0] = jacob;
    // ppl.people[1] = {
    //     .name = "Swapnil", .age = 26, .lucky_numbers = {1, 2, 3, 4}};
    // ppl.people[2] = {.name = "Noah", .age = 40, .lucky_numbers = {1, 2, 3,
    // 4}}; ppl.people[3] = {.name = "Hey", .age = 40, .lucky_numbers = {1, 2,
    // 3, 4}};

    ppl.nicknames[0] = "Jakey";
    ppl.nicknames[1] = "Swapnilly";
    ppl.nicknames[2] = "Noey";
    ppl.nicknames[3] = "Heyey";

    Person p = nth_person(ppl, 0);

    printf("name: %s age: %d luckys: %d %d %d %d", p.name.c_str(), p.age,
           p.lucky_numbers[0], p.lucky_numbers[1], p.lucky_numbers[2],
           p.lucky_numbers[3]);

    float avg = avg_4D(nums);

    DEBUG("%f", avg);

    return 0;
}
