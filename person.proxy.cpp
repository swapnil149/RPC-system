#include <stdio.h>
#include <string>
using namespace std;
#include "common/basis.h"
#include "common/mem.h"
#include "common/transmit.h"
#include "person.idl"

void __pack_Person(Person bandhi, __rpcmem_t *mem) {
  __pack_string(bandhi.name, mem);
  __pack_int(bandhi.age, mem);
  for (int i = 0; i < 5; i++) {
    __pack_int(bandhi.lucky_numbers[i], mem);
  }
}

void __pack_People(People bandhi, __rpcmem_t *mem) {
  for (int i = 0; i < 4; i++) {
    __pack_Person(bandhi.people[i], mem);
  }
  for (int i = 0; i < 4; i++) {
    __pack_string(bandhi.nicknames[i], mem);
  }
}

Person __unpack_Person(__rpcmem_t *mem) {
  Person result;
  for (int i = 5; i > 0; i--) {
    result.lucky_numbers[i - 1] = __unpack_int(mem);
  }
  result.age = __unpack_int(mem);
  result.name = __unpack_string(mem);
  return result;
}

People __unpack_People(__rpcmem_t *mem) {
  People result;
  for (int i = 4; i > 0; i--) {
    result.nicknames[i - 1] = __unpack_string(mem);
  }
  for (int i = 4; i > 0; i--) {
    result.people[i - 1] = __unpack_Person(mem);
  }
  return result;
}

float avg_4D(int nums[8][8][8][8]) {
  __rpcmem_t *mem = rpcmem_new();
  char fname[MAX_FNAME_LEN] = "avg_4D";

  for (int i = 0; i < 8 * 8 * 8 * 8; i++) {
    __pack_int(nums[0][0][0][i], mem);
  }

  rpc_send(fname, mem);
  rpc_recv(fname, mem);

  float result = __unpack_float(mem);

  rpcmem_free(&mem);
  return result;
}

Person nth_person(People ppl, int n) {
  __rpcmem_t *mem = rpcmem_new();
  char fname[MAX_FNAME_LEN] = "nth_person";

  __pack_People(ppl, mem);
  __pack_int(n, mem);

  rpc_send(fname, mem);
  rpc_recv(fname, mem);

  Person result = __unpack_Person(mem);

  rpcmem_free(&mem);
  return result;
}
