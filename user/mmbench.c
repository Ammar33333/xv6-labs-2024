#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static uint64 now_cycles() { return getcycle(); }
static uint64 now_time()   { return gettime(); }
static uint64 now_inst()   { return getinstret(); }

// naive i-j-k triple loop matmul
static void matmul(int *A, int *B, int *C, int n) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      long long s = 0;
      for (int k = 0; k < n; k++)
        s += (long long)A[i*n + k] * (long long)B[k*n + j];
      C[i*n + j] = (int)s;
    }
  }
}

static void fill(int *M, int n, int seed) {
  for (int i = 0; i < n*n; i++) M[i] = (i + seed) & 1023;
}

static int try_one(int n) {
  // 3 matrices of n*n ints each
  uint64 bytes = (uint64)n * (uint64)n * sizeof(int);
  uint64 need  = bytes * 3;

  // ask for memory in one chunk (simple) using malloc
  int *A = (int*)malloc(bytes);
  if (!A) return -1;
  int *B = (int*)malloc(bytes);
  if (!B) { free(A); return -1; }
  int *C = (int*)malloc(bytes);
  if (!C) { free(B); free(A); return -1; }

  fill(A, n, 1);
  fill(B, n, 7);

  // measure
  uint64 c0 = now_cycles();
  uint64 t0 = now_time();
  uint64 i0 = now_inst();

  matmul(A, B, C, n);

  uint64 c1 = now_cycles();
  uint64 t1 = now_time();
  uint64 i1 = now_inst();

  printf("N=%d  bytes=%ld  cycles=%ld  time=%ld  inst=%ld\n",
         n, need, (c1 - c0), (t1 - t0), (i1 - i0));

  free(C); free(B); free(A);
  return 0;
}

int
main(int argc, char **argv)
{
  // sizes to try; safe with 128MB; we stop if alloc fails
  int sizes[] = {256, 512, 768, 1024, 1536, 2048};
  int m = sizeof(sizes)/sizeof(sizes[0]);

  printf("mmbench: cycles/time/instret via syscalls\n");
  for (int idx = 0; idx < m; idx++) {
    if (try_one(sizes[idx]) < 0) {
      printf("N=%d  allocation failed — stopping.\n", sizes[idx]);
      break;
    }
  }
  exit(0);
}
