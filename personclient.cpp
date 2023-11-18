#include "person.proxy.cpp"

int rpc_main(int argc, char *argv[]) {
  (void)argc, (void)argv;

  int nums[8][8][8][8];

  memset(nums, 88, 8 * 8 * 8 * 8 * 4);

  float result = avg_4D(nums);
  printf("result: %f\n", result);

  return 0;
}
