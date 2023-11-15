#include <cstring>
#include <stdio.h>
#include <string>
using namespace std;

#include "person.idl"

#include <cstdlib>

#include "common/basis.h"
#include "common/mem.h"

#include "c150debug.h"
#include "samples/rpcstubhelper.h"

// packers
// ...

// unpackers
// ...

// stub functions
// ...

void dispatchFunction() {
  // listens
  // TODO: steal code from Noah
  char *buf;
  char fname[MAX_FNAME_LEN];
  int len = recv(&buf);

  rpcmem_t *m = rpcmem_new();

  rpcmem_frombuf(buf, len, fname, m);

  if (strcmp(fname, "func1")) {
    __func1(m);
    goto cleanup;
  }

  // ... repeat this for each function

  exit(1);

cleanup:
  rpcmem_free(&m);
}
