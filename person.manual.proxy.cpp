#include <stdio.h>
#include <string>
using namespace std;

#include "person.idl"

#include "c150debug.h"
#include <cstdlib>

#include "common/basis.h"
#include "common/mem.h"
#include "common/transmit.h"

#include "samples/rpcproxyhelper.h"

using namespace C150NETWORK; // for all the comp150 utilities

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

// proxy functions
Person nth_person(People ppl, int n) {
  rpcmem_t *mem = rpcmem_new();
  char *buf;
  char fname[MAX_FNAME_LEN];

  pack_People(ppl, mem);
  pack_int(n, mem);

  int len = rpcmem_tobuf("nth_person", mem, &buf);
  rpc_send(buf, len);

  len = rpc_recv(buf);
  rpcmem_frombuf(buf, len, fname, mem);

  Person result;
  result = unpack_Person(mem);

  rpcmem_free(&mem);
  free(buf);

  return result;
}

float avg_4D(int nums[8][8][7][8]) {
  rpcmem_t *mem = rpcmem_new();
  char *buf;
  char fname[MAX_FNAME_LEN];

  for (int i = 0; i < (8 * 8 * 7 * 8); i++)
    pack_int(nums[0][0][0][i], mem);

  int len = rpcmem_tobuf("avg_4D", mem, &buf);
  rpc_send(buf, len);

  len = rpc_recv(buf);
  rpcmem_frombuf(buf, len, fname, mem);

  float result;
  result = unpack_float(mem);

  rpcmem_free(&mem);
  free(buf);

  return result;
}
