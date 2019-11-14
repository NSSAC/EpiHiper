// BEGIN: Copyright 
// Copyright (C) 2019 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include "utilities/CRandom.h"
#include "utilities/CCommunicate.h"

#define NUM_ELEMENT  4
long  L = 0;

int main(int argc, char** argv)
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

int __main__(int argc, char** argv)
{
  int i, len, localbuffer[NUM_ELEMENT], sharedbuffer[NUM_ELEMENT];

  CRandom::generator_t R;
  R.seed(std::random_device()());
  CRandom::uniform_int uniform(0, 100);

  CCommunicate::init(argc, argv);

  char name[MPI_MAX_PROCESSOR_NAME];
  MPI_Win win, gvwin;

  MPI_Get_processor_name(name, &len);

  printf("Rank %d running on %s\n", CCommunicate::MPIRank, name);
  printf("Rank %d: previous %d, next %d\n", CCommunicate::MPIRank, CCommunicate::MPIPreviousRank, CCommunicate::MPINextRank);

  MPI_Win_create(sharedbuffer, NUM_ELEMENT, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

  for (i = 0; i < NUM_ELEMENT; i++)
    {
      sharedbuffer[i] = 10*CCommunicate::MPIRank + i;
      localbuffer[i] = 0;
    }

  printf("Rank %d sets data in the shared memory:", CCommunicate::MPIRank);

  for (i = 0; i < NUM_ELEMENT; i++)
    printf(" %02d", sharedbuffer[i]);

  printf("\n");

  MPI_Win_fence(0, win);
  MPI_Get(&localbuffer[0], NUM_ELEMENT, MPI_INT, CCommunicate::MPIPreviousRank, 0, NUM_ELEMENT, MPI_INT, win);
  MPI_Win_fence(0, win);

  printf("Rank %d gets data from the shared memory:", CCommunicate::MPIRank);

  for (i = 0; i < NUM_ELEMENT; i++)
    printf(" %02d", localbuffer[i]);

  printf("\n");

  MPI_Win_fence(0, win);
  MPI_Put(&localbuffer[0], NUM_ELEMENT, MPI_INT, CCommunicate::MPINextRank, 0, NUM_ELEMENT, MPI_INT, win);
  MPI_Win_fence(0, win);

  printf("Rank %d has new data in the shared memory:", CCommunicate::MPIRank);

  for (i = 0; i < NUM_ELEMENT; i++)
    printf(" %02d", sharedbuffer[i]);

  printf("\n");

  int local, global, *pShared, shared, result;

  local = 0;
  global = 0;

  // MPI_Alloc_mem(sizeof(int), MPI_INFO_NULL, &pShared);

  result = MPI_Win_create(&shared, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &gvwin);

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

  for (i = 0; i < 1000; ++i)
    {
      usleep(uniform(R));

      MPI_Win_sync(gvwin);
      MPI_Get(&global, 1, MPI_INT, 0, 0, 1, MPI_INT, gvwin);

      if (global < 100 &&
          uniform(R) < 20)
        {

          // MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, gvwin);
          // MPI_Win_fence(0, gvwin);
          int ONE = 1;
          MPI_Accumulate(&ONE, 1, MPI_INT, 0, 0, 1, MPI_INT, MPI_SUM, gvwin);
          /*
           MPI_Get(&global, 1, MPI_INT, 0, sizeof(int), 1, MPI_INT, gvwin);
           global += 1;
           MPI_Put(&global, 1, MPI_INT, 0, sizeof(int), 1, MPI_INT, gvwin);
           */

          // MPI_Win_flush_all(gvwin);
          // MPI_Win_fence(0, gvwin);
          // MPI_Win_unlock(0, gvwin);
          local += 1;
        }
    }

  MPI_Win_fence(0, gvwin);
  MPI_Get(&global, 1, MPI_INT, 0, 0, 1, MPI_INT, gvwin);
  MPI_Win_fence(0, gvwin);

  printf("Rank %d has final gobal value: %d\n", CCommunicate::MPIRank, global);
  printf("Rank %d has final local value: %d\n", CCommunicate::MPIRank, local);


  MPI_Win_free(&gvwin);
  MPI_Win_free(&win);

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
  return 0;
}
