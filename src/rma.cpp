// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions: 
//  
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software. 
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE 
// END: Copyright 

#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include "utilities/CRandom.h"
#include "utilities/CCommunicate.h"

#define NUM_ELEMENT  4
/* tests passive target RMA on 2 processes */
#define SIZE1 100
#define SIZE2 200

long  L = 0;
#define TEST_MPI_WIN 1

#ifdef XXXX
int __main__(int argc, char** argv)
{
  CRandom::generator_t R;
  R.seed(std::random_device()());
  CRandom::uniform_int uniform(0, 1000);

  static long lock = 0;
  static int count = 0;

  CCommunicate::init(argc, argv);

  for (int i = 0; i < 100; ++i)
    {
      std::this_thread::sleep_for (std::chrono::milliseconds(uniform(R)));

      shmem_set_lock(&lock);
      int val = shmem_int_fetch(&count, 0); /* get count value on PE 0 */
      printf("%d: count is %d\n", CCommunicate::MPIRank, val);
      val++; /* incrementing and updating count on PE 0 */
      shmem_int_p(&count, val, 0);
      shmem_quiet();
      shmem_clear_lock(&lock);
    }

  shmem_finalize();

  return 0;
}
#endif // XXXX

int main(int argc, char** argv)
{
#ifdef TEST_EXAMPLE
  int rank, nprocs, A[SIZE2], B[SIZE2], i;
  MPI_Win win;
  int errs = 0;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  if (nprocs != 2) {
      printf("Run this program with 2 processes\n");fflush(stdout);
      MPI_Abort(MPI_COMM_WORLD,1);
  }

  if (rank == 0) {
      for (i=0; i<SIZE2; i++) A[i] = B[i] = i;
      MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
      MPI_Win_fence(0, win);
      for (i=0; i<SIZE1; i++) {
          MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win);
          MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win);
          MPI_Put(A+i, 1, MPI_INT, 1, i, 1, MPI_INT, win);
          MPI_Win_unlock(1, win);
      }

      for (i=0; i<SIZE1; i++) {
          MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win);
          MPI_Get(B+i, 1, MPI_INT, 1, SIZE1+i, 1, MPI_INT, win);
          MPI_Win_unlock(1, win);
      }

      MPI_Win_free(&win);

      for (i=0; i<SIZE1; i++)
        if (B[i] != (-4)*(i+SIZE1)) {
            printf("Get Error: B[%d] is %d, should be %d\n", i, B[i], (-4)*(i+SIZE1));fflush(stdout);
            errs++;
        }
  }
  else {  /* rank=1 */
      for (i=0; i<SIZE2; i++) B[i] = (-4)*i;
      MPI_Win_create(B, SIZE2*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
      MPI_Win_fence(0, win);

      MPI_Win_free(&win);

      for (i=0; i<SIZE1; i++) {
          if (B[i] != i) {
              printf("Put Error: B[%d] is %d, should be %d\n", i, B[i], i);
              errs++;
          }
      }
  }

  MPI_Finalize();
  return errs;
#endif // TEST_EXAMPLE

#ifdef TEST_MPI_WIN
  int i, len, localbuffer[NUM_ELEMENT], sharedbuffer[NUM_ELEMENT];

  CRandom::generator_t R;
  R.seed(std::random_device()());
  CRandom::uniform_int uniform(0, 100);

  CCommunicate::init(argc, argv);

  char name[MPI_MAX_PROCESSOR_NAME];

  MPI_Win gvwin;

  MPI_Get_processor_name(name, &len);

  printf("Rank %d running on %s\n", CCommunicate::MPIRank, name);
  printf("Rank %d: previous %d, next %d\n", CCommunicate::MPIRank, CCommunicate::MPIPreviousRank, CCommunicate::MPINextRank);

  int local, global, *pShared, shared, result;

  local = 0;
  global = 0;

  // MPI_Alloc_mem(sizeof(int), MPI_INFO_NULL, &pShared);

  result = MPI_Win_create(&shared, 1, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &gvwin);

  if (result)
    printf("Rank %d (%d) has result: %d\n", CCommunicate::MPIRank, __LINE__, result);

  result = MPI_Win_fence(0, gvwin);

  if (result)
    printf("Rank %d (%d) has result: %d\n", CCommunicate::MPIRank, __LINE__, result);

  if (CCommunicate::MPIRank == 0)
    {
      result = MPI_Put(&global, 1, MPI_INT, 0, 0, 1, MPI_INT, gvwin);

      if (result)
        printf("Rank %d (%d) has result: %d\n", CCommunicate::MPIRank, __LINE__, result);

      printf("Rank %d has shared gobal value: %d\n", CCommunicate::MPIRank, shared);
    }

  MPI_Win_fence(0, gvwin);
  MPI_Get(&global, 1, MPI_INT, 0, 0, 1, MPI_INT, gvwin);
  MPI_Win_fence(0, gvwin);

  printf("Rank %d has new gobal value: %d\n", CCommunicate::MPIRank, global);

  for (i = 0; i < 1000 && global < 100; ++i)
    {
      // std::this_thread::sleep_for(std::chrono::microseconds(uniform(R)));

      if (uniform(R) < 20)
        {
          MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, gvwin);
          MPI_Get(&global, 1, MPI_INT, 0, 0, 1, MPI_INT, gvwin);
          MPI_Win_flush(0, gvwin);

          if (global < 100)
            {
              global += 1;
              local += 1;
              printf("Rank %d has global value: %d\n", CCommunicate::MPIRank, global);
              MPI_Put(&global, 1, MPI_INT, 0, 0, 1, MPI_INT, gvwin);
              MPI_Win_flush(0, gvwin);
            }

          MPI_Win_unlock(0, gvwin);
        }
    }

  MPI_Win_fence(0, gvwin);
  MPI_Get(&global, 1, MPI_INT, 0, 0, 1, MPI_INT, gvwin);
  MPI_Win_fence(0, gvwin);

  printf("Rank %d has final gobal value: %d\n", CCommunicate::MPIRank, global);
  printf("Rank %d has final local value: %d\n", CCommunicate::MPIRank, local);


  MPI_Win_free(&gvwin);

  // CCommunicate::finalize();
#endif // TEST_MPI_WIN

#ifdef TEST_OSHMEM
  int i, len, localbuffer[NUM_ELEMENT], sharedbuffer[NUM_ELEMENT];

  CRandom::generator_t R;
  R.seed(std::random_device()());
  CRandom::uniform_int uniform(0, 100);

  CCommunicate::init(argc, argv);

  char name[MPI_MAX_PROCESSOR_NAME];

  std::cout << CCommunicate::MPIRank << ":" << __LINE__ << std::endl;

  int major, minor;
  std::cout << CCommunicate::MPIRank << ":" << __LINE__ << std::endl;

  shmem_info_get_version(&major, &minor);
  std::cout << CCommunicate::MPIRank << ":" << __LINE__ << std::endl;

  printf("Rank %d Version %d.%d\n", CCommunicate::MPIRank, major, minor);

  std::cout << CCommunicate::MPIRank << ":" << __LINE__ << std::endl;

  double * pshGlobal = NULL;
  pshGlobal = (double *) shmem_malloc(sizeof(double));
  *pshGlobal = -1.0;

  std::cout << CCommunicate::MPIRank << ":" << __LINE__ << std::endl;
  long * pshLock = (long *) shmem_malloc(sizeof(long));
  *pshLock = 0;

  std::cout << CCommunicate::MPIRank << ":" << __LINE__ << std::endl;

  double Global = -1.0;
  double Local = 0.0;

  if (CCommunicate::MPIRank == 0)
    {
      Global = 0.0;
      shmem_double_p(pshGlobal, Global, 0);
      printf("Rank %d has shared gobal value: %g\n", CCommunicate::MPIRank, *pshGlobal);
    }

  std::cout << CCommunicate::MPIRank << ":" << __LINE__ << std::endl;

  shmem_fence();

  std::cout << CCommunicate::MPIRank << ":" << __LINE__ << std::endl;

  Global = shmem_double_g(pshGlobal, 0);

  std::cout << CCommunicate::MPIRank << ":" << __LINE__ << std::endl;

  printf("Rank %d has new gobal value: %g\n", CCommunicate::MPIRank, Global);

  shmem_barrier_all();

  std::cout << CCommunicate::MPIRank << ":" << __LINE__ << std::endl;

  double ONE = 1.0;
  for (i = 0; i < 1000; ++i)
    {
      std::this_thread::sleep_for (std::chrono::microseconds(uniform(R)));

      std::cout << CCommunicate::MPIRank << ":" << __LINE__ << ":" << i << std::endl;

      Global = shmem_double_g(pshGlobal, 0);

      std::cout << CCommunicate::MPIRank << ":" << __LINE__ << ":" << i << std::endl;

      if (Global >= 100) break;

      if (uniform(R) < 20)
        {
          std::cout << CCommunicate::MPIRank << "@" << __LINE__ << ":" << i << std::endl;
          shmem_set_lock(pshLock);
          std::cout << CCommunicate::MPIRank << "@" << __LINE__ << ":" << i << std::endl;

          Global = shmem_double_g(pshGlobal, 0);
          if (Global >= 100) break;
          Global += 1.0;
          shmem_double_p(pshGlobal, Global, 0);

          std::cout << CCommunicate::MPIRank << "@" << __LINE__ << ":" << i << std::endl;
          shmem_quiet();
          std::cout << CCommunicate::MPIRank << "@" << __LINE__ << ":" << i << std::endl;
          shmem_clear_lock(pshLock);
          std::cout << CCommunicate::MPIRank << "@" << __LINE__ << ":" << i << std::endl;

          Local += 1.0;
        }
    }

  shmem_fence();

  Global = shmem_double_g(pshGlobal, 0);

  printf("Rank %d has final gobal value: %g\n", CCommunicate::MPIRank, Global);
  printf("Rank %d has final local value: %g\n", CCommunicate::MPIRank, Local);

  CCommunicate::finalize();
#endif // TEST_OSHMEM
  return 0;
}
