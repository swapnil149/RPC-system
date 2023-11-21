#include <string>
using namespace std;
#include "person.idl"

Person nth_person(People ppl, int n) { return ppl.people[n]; }

float avg_4D(int nums[8][8][8][8]) {
    float sum = 0.0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            for (int k = 0; k < 8; k++) {
                for (int q = 0; q < 8; q++) {
                    sum += nums[i][j][k][q];
                }
            }
        }
    }
    return sum / (float)(8 * 8 * 8 * 8);
}
