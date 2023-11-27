#include <stdlib.h>
#include <string>
using namespace std;

#include "debugmacros.h"
#include "person.idl"

int rpc_main(int argc, char **argv) {
    (void)argc, (void)argv;

    int nums[8][8][8][8];
    People ppl;

    for (int i = 0; i < 8 * 8 * 8 * 8; i++)
        nums[0][0][0][i] = 20 + i;

    ppl.people[0] = {"Jacob", 23, {1, 2, 3, 4}};
    ppl.people[1] = {"Swapnil", 27, {21, 72, 83, 234}};
    ppl.people[2] = {"Noah", 62, {21, 22, 33, 44}};
    ppl.people[3] = {"Hey", 12, {18, 8, 32, 41}};
    ppl.nicknames[0] = "Jakey";
    ppl.nicknames[1] = "Swapnilly";
    ppl.nicknames[2] = "Noey";
    ppl.nicknames[3] = "Heyey";

    int n = 0;
    DEBUG("%d", n);
    Person p = nth_person(ppl, n);
    DEBUG("%s", p.name.c_str());
    DEBUG("%d", p.age);
    for (int i = 0; i < 4; i++) {
        DEBUG("%d", p.lucky_numbers[i]);
    }

    n = 1;
    DEBUG("%d", n);
    p = nth_person(ppl, n);
    DEBUG("%s", p.name.c_str());
    DEBUG("%d", p.age);
    for (int i = 0; i < 4; i++)
        DEBUG("%d", p.lucky_numbers[i]);

    n = 2;
    DEBUG("%d", n);
    p = nth_person(ppl, n);
    DEBUG("%s", p.name.c_str());
    DEBUG("%d", p.age);
    for (int i = 0; i < 4; i++)
        DEBUG("%d", p.lucky_numbers[i]);

    n = 3;
    DEBUG("%d", n);
    p = nth_person(ppl, n);
    DEBUG("%s", p.name.c_str());
    DEBUG("%d", p.age);
    for (int i = 0; i < 4; i++)
        DEBUG("%d", p.lucky_numbers[i]);

    float avg = avg_4D(nums);
    DEBUG("%f", avg);

    return 0;
}
