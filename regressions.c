
#include "debugmacros.h"
#include "rpcmem.h"

int main() {
  rpcmem_t *MEM = rpcmem_new();
  int x = 4487, y = -4322;
  bool b = true;
  rpcptr_t ptr = 0xfffa;

  int arr[] = {5, 7, 8, 2, 8, 123, 21};
  int triple_array[8][3][4] = {0};

  // fill array with shittyrand(TM) algorithm
  for (int i = 0; i < sizeof(triple_array) / 4; i++)
    triple_array[0][0][i] = (i + 5) * (i + 0xacb23143) << (i % 4);

  // print some array values
  DEBUGINT(arr[3]);
  DEBUGINT(triple_array[7][2][3]);
  DEBUGINT(triple_array[6][2][2]);
  DEBUGINT(triple_array[0][0][0]);

  // push to stack
  DEBUGINT(RSP);
  PUSH(INT, x);
  DEBUGINT(RSP);
  PUSH(INT, y);
  DEBUGINT(RSP);
  PUSH(RPCPTR, ptr);
  DEBUGINT(RSP);

  SBRK(34);

  PUSH(BOOL, true);
  DEBUGINT(RSP);
  PUSH(INT, arr, 40);
  DEBUGINT(RSP);
  PUSH(INT, triple_array, 8, 3, 4);
  DEBUGINT(RSP);

  // change values
  b = false;
  ptr = -1;
  x = -1;
  y = -1;
  memset(arr, -1, sizeof(arr));
  memset(triple_array, -1, sizeof(triple_array));

  // sanity check: print some changed array values
  DEBUGINT(arr[3]);
  DEBUGINT(triple_array[7][2][3]);
  DEBUGINT(triple_array[6][2][2]);
  DEBUGINT(triple_array[0][0][0]);

  char *buf;
  int len = rpcmem_tobuf("regression_test", MEM, &buf);

  // reset memory unit
  RSP = -1;
  RHP = -1;
  // memset(RDATA, -1, MEM_CAPACITY);

  char fname[MAX_IDL_FNAME_LEN];
  rpcmem_frombuf(buf, len, MEM, fname);

  fprintf(stderr, "received: %s\n", fname);

  // pop off the stack
  DEBUGINT(RSP);
  POP(INT, triple_array, 8, 3, 4);
  DEBUGINT(RSP);
  POP(INT, arr, 40);
  DEBUGINT(RSP);
  POP(BOOL, b);
  DEBUGINT(RSP);
  POP(RPCPTR, ptr);
  DEBUGINT(RSP);
  POP(INT, y);
  DEBUGINT(RSP);
  POP(INT, x);
  DEBUGINT(RSP);

  // print extracted values
  DEBUGINT(arr[3]);
  DEBUGINT(triple_array[7][2][3]);
  DEBUGINT(triple_array[6][2][2]);
  DEBUGINT(triple_array[0][0][0]);

  DEBUGBOOL(b);
  DEBUGPTR(ptr);
  DEBUGINT(x);
  DEBUGINT(y);

  rpcmem_free(&MEM);
  free(buf);

  return 0;
}
