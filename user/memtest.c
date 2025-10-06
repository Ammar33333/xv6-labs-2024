#include "kernel/types.h"
#include "user/user.h"

int main(void) {
  printf("Allocating memory...\n");
  void *p;
  int count = 0;
  while ((p = malloc(4096)) != 0) {
    count++;
    if (count % 1000 == 0)
      printf("Allocated %d pages (~%d KB)\n", count, count * 4);
  }
  printf("Done! Allocated total %d pages (~%d MB)\n", count, count * 4 / 1024);
  exit(0);
}
