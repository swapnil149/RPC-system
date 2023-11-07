#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "rpcmem.h"

#define DEBUGINT(x) fprintf(stderr, #x ": %d\n", x);
#define DEBUGBOOL(x) fprintf(stderr, #x ": %s\n", x ? "true" : "false");
#define DEBUGPTR(x) fprintf(stderr, #x ": 0x%04x\n", x);

int main() {
  rpcmem_t *MEM = rpcmem_new();

  int x = 4487, y = -4322;
  bool b = true;
  rpcptr_t ptr = 0x41;

  int arr[] = {5, 7, 8, 2, 8, 123, 21};
  int triple_array[8][3][4] = {0};

  for (int i = 0; i < sizeof(triple_array) / 4; i++) {
    triple_array[0][0][i] = i;
  }

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

  DEBUGINT(arr[3]);
  DEBUGINT(triple_array[6][1][2]);
  DEBUGINT(triple_array[6][2][2]);

  // pop back off
  POP(INT, triple_array, 8, 3, 4);
  POP(INT, arr, 40);
  POP(BOOL, b);
  POP(RPCPTR, ptr);
  POP(INT, y);
  POP(INT, x);

  DEBUGINT(arr[3]);
  DEBUGINT(triple_array[6][1][2]);
  DEBUGINT(triple_array[6][2][2]);

  DEBUGBOOL(b);
  DEBUGPTR(ptr);
  DEBUGINT(x);
  DEBUGINT(y);

  rpcmem_free(&MEM);
  return 0;
}
