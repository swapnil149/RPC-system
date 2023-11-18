#include <stdio.h>
#include <string>
using namespace std;
#include "common/basis.h"
#include "common/mem.h"
#include "person.idl"
#include "rpcstubhelper.h"
#define RPCSOCKET RPCSTUBSOCKET
#include "common/transmit.h"

void __pack_Person(Person bandhi, __rpcmem_t *mem) {
  __pack_string(bandhi.name, mem);
  __pack_int(bandhi.age, mem);
  for (int __i = 0; __i < 5; __i++) {
    __pack_int(bandhi.lucky_numbers[__i], mem);
  }
}

void __pack_People(People bandhi, __rpcmem_t *mem) {
  for (int __i = 0; __i < 4; __i++) {
    __pack_Person(bandhi.people[__i], mem);
  }
  for (int __i = 0; __i < 4; __i++) {
    __pack_string(bandhi.nicknames[__i], mem);
  }
}

Person __unpack_Person(__rpcmem_t *mem) {
  Person result;
  for (int __i = 5; __i > 0; __i--) {
    result.lucky_numbers[__i - 1] = __unpack_int(mem);
  }
  result.age = __unpack_int(mem);
  result.name = __unpack_string(mem);
  return result;
}

People __unpack_People(__rpcmem_t *mem) {
  People result;
  for (int __i = 4; __i > 0; __i--) {
    result.nicknames[__i - 1] = __unpack_string(mem);
  }
  for (int __i = 4; __i > 0; __i--) {
    result.people[__i - 1] = __unpack_Person(mem);
  }
  return result;
}

void __avg_4D(__rpcmem_t *mem) {
  int nums[8][8][8][8];
  for (int __i = 8 * 8 * 8 * 8; __i > 0; __i--) {
    nums[0][0][0][__i - 1] = __unpack_int(mem);
  }
  float result = avg_4D(nums);
  mem->hp = 0;
  mem->sp = mem->capacity;
  __pack_float(result, mem);
}

void __nth_person(__rpcmem_t *mem) {
  People ppl;
  int n;
  n = __unpack_int(mem);
  ppl = __unpack_People(mem);
  Person result = nth_person(ppl, n);
  mem->hp = 0;
  mem->sp = mem->capacity;
  __pack_Person(result, mem);
}

// dispatcher
