#include <cstring>
#include <stdio.h>
#include <string>
using namespace std;

#include "person.idl"

// #include "c150debug.h"
#include <cstdlib>

#include "common/basis.h"
#include "common/mem.h"
#include "common/transmit.h"

// #include "samples/rpcstubhelper.h"

//__packers
void __pack_Person(Person p, __rpcmem_t *mem) {
  __pack_string(p.name, mem);
  __pack_int(p.age, mem);
  for (int i = 0; i < 5; i++) {
    __pack_int(p.lucky_numbers[i], mem);
  }
}

void __pack_People(People p, __rpcmem_t *mem) {
  for (int i = 0; i < 4; i++) {
    __pack_Person(p.people[i], mem);
  }
  for (int i = 0; i < 4; i++) {
    __pack_string(p.nicknames[i], mem);
  }
}

//__unpackers
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

// stub functions
void __nth_person(__rpcmem_t *mem) {
  People ppl;
  int n;
  ppl = __unpack_People(mem);
  n = __unpack_int(mem);

  mem->hp = 0;
  mem->sp = mem->capacity;

  Person result = nth_person(ppl, n);
  __pack_Person(result, mem);
}

void __avg_4D(__rpcmem_t *mem) {
  int nums[8][8][8][8];

  for (int i = (8 * 8 * 8 * 8); i > 0; i--) {
    nums[0][0][0][i - 1] = __unpack_int(mem);
  }

  mem->hp = 0;
  mem->sp = mem->capacity;

  float result = avg_4D(nums);
  __pack_float(result, mem);
}

// dispatch function
void dispatchFunction() {
  __rpcmem_t *mem = rpcmem_new();
  char fname[MAX_FNAME_LEN];

  int len = rpc_recv(fname, mem);

  if (strcmp(fname, "nth_person")) {
    __nth_person(mem);
    goto respond;
  }

  if (strcmp(fname, "nth_person")) {
    ...;
    goto respond;
  }

  exit(1);

respond:
  // send stuff
  rpc_send(fname, mem);
}
