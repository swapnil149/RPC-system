
#include "debugmacros.h"
#include "rpcmem.h"

int main() {
  int x = 4487, y = -4322;
  bool b = true;
  rpcptr_t ptr = 0x41;

  int arr[] = {5, 7, 8, 2, 8, 123, 21};
  int triple_array[8][3][4] = {0};

  // fill array
  for (int i = 0; i < sizeof(triple_array) / 4; i++)
    triple_array[0][0][i] = i;

  // print some array values
  DEBUGINT(arr[3]);
  DEBUGINT(triple_array[6][1][2]);
  DEBUGINT(triple_array[6][2][2]);

  // push to stack
  PUSH(INT, x);
  PUSH(INT, y);
  PUSH(RPCPTR, ptr);
  PUSH(BOOL, true);
  PUSH(INT, arr, 40);
  PUSH(INT, triple_array, 8, 3, 4);

  // change values
  b = false;
  ptr = -1;
  x = -1;
  y = -1;
  memset(arr, -1, sizeof(arr));
  memset(triple_array, -1, sizeof(triple_array));

  // sanity check: print some changed array values
  DEBUGINT(arr[3]);
  DEBUGINT(triple_array[6][1][2]);
  DEBUGINT(triple_array[6][2][2]);

  rpcmem_tobuf("regression_test", MEM, global_transmit_buffer);

  // reset memory unit
  RSP = -1;
  RHP = -1;
  memset(RDATA, -1, MEM_CAPACITY);

  char fname[MAX_IDL_FNAME_LEN];
  rpcmem_frombuf(global_transmit_buffer, MEM, fname);

  fprintf(stderr, "received: %s\n", fname);

  // pop off the stack
  POP(INT, triple_array, 8, 3, 4);
  POP(INT, arr, 40);
  POP(BOOL, b);
  POP(RPCPTR, ptr);
  POP(INT, y);
  POP(INT, x);

  // print extracted values
  DEBUGINT(arr[3]);
  DEBUGINT(triple_array[6][1][2]);
  DEBUGINT(triple_array[6][2][2]);

  DEBUGBOOL(b);
  DEBUGPTR(ptr);
  DEBUGINT(x);
  DEBUGINT(y);

  return 0;
}
