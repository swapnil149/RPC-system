#include <cstring>
#include <stdio.h>
#include <string>
using namespace std;

#include "person.idl"

#include "c150debug.h"
#include <cstdlib>

#include "common/basis.h"
#include "common/mem.h"
#include "common/transmit.h"

#include "samples/rpcstubhelper.h"

// packers
void pack_Person(Person p, rpcmem_t *mem) {
  pack_string(p.name, mem);
  pack_int(p.age, mem);
  for (int i = 0; i < 5; i++) {
    pack_int(p.lucky_numbers[i], mem);
  }
}

void pack_People(People p, rpcmem_t *mem) {
  for (int i = 0; i < 4; i++) {
    pack_Person(p.people[i], mem);
  }
  for (int i = 0; i < 4; i++) {
    pack_string(p.nicknames[i], mem);
  }
}

// unpackers
Person unpack_Person(rpcmem_t *mem) {
  Person result;
  for (int i = 5; i > 0; i--) {
    result.lucky_numbers[i - 1] = unpack_string(mem);
  }
  result.age = unpack_int(mem);
  result.name = unpack_string(mem);
  return result;
}

People unpack_People(rpcmem_t *mem) {
  People result;
  for (int i = 4; i > 0; i--) {
    result.nicknames[i - 1] = unpack_string(mem);
  }
  for (int i = 4; i > 0; i--) {
    result.people[i - 1] = unpack_Person(mem);
  }
  return result;
}

// stub functions
void __nth_person(rpcmem_t *mem) {
  People ppl;
  int n;

  ppl = unpack_People(mem);
  n = unpack_int(mem);

  mem->hp = 0;
  mem->sp = mem->capacity;

  Person result = nth_person(ppl, n);
  pack_Person(result, mem);
}

void __avg_4D(rpcmem_t *mem) {
  int nums[8][8][7][8];

  for (int i = (8 * 8 * 7 * 8); i > 0; i--) {
    nums[0][0][0][i - 1] = unpack_int(mem);
  }

  mem->hp = 0;
  mem->sp = mem->capacity;

  float result = avg_4D(nums);
  pack_float(result, mem);
}

// dispatch function
void dispatchFunction() {
  rpcmem_t *mem = rpcmem_new();
  char fname[MAX_FNAME_LEN];

  int len = rpc_recv(buf);
  rpcmem_frombuf(buf, len, fname, mem);

  if (stcmp(fname, "nth_person")) {
    __nth_person(mem);
    goto respond;
  }

  if (stcmp(fname, "nth_person")) {
    ...;
    goto respond;
  }

  throw error;
  exit(1);

respond:
  // send stuff
  rpc_send(const char *inbuf, int len)
}
