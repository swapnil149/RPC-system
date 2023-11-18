#include <stdio.h>
#include <string>
using namespace std;
#include "person.idl"
#include "rpcproxyhelper.h"
#define RPCSOCKET RPCPROXYSOCKET
#include "common/basis.h"
#include "common/mem.h"
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

float avg_4D(int nums[8][8][8][8]) {
  __rpcmem_t *mem = rpcmem_new();

  for (int __i = 0; __i < 8 * 8 * 8 * 8; __i++) {
    __pack_int(nums[0][0][0][__i], mem);
  }

  rpc_send("avg_4D", mem);
  rpc_recv(mem);

  float result = __unpack_float(mem);

  rpcmem_free(&mem);
  return result;
}

Person nth_person(People ppl, int n) {
  __rpcmem_t *mem = rpcmem_new();

  __pack_People(ppl, mem);
  __pack_int(n, mem);

  rpc_send("nth_person", mem);
  rpc_recv(mem);

  Person result = __unpack_Person(mem);

  rpcmem_free(&mem);
  return result;
}
