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
#include "math/CValueInterface.h"

#define NUM_ELEMENT  4
/* tests passive target RMA on 2 processes */
#define SIZE1 100
#define SIZE2 200

long  L = 0;
#define TEST_MPI_WIN 1

int main(int argc, char** argv)
{
  int i, len, localbuffer[NUM_ELEMENT], sharedbuffer[NUM_ELEMENT];

  CRandom::generator_t R;
  R.seed(std::random_device()());
  CRandom::uniform_int uniform(0, 100);

  CCommunicate::init(argc, argv);

  char name[MPI_MAX_PROCESSOR_NAME];

  MPI_Get_processor_name(name, &len);

  printf("Rank %d running on %s\n", CCommunicate::MPIRank, name);
  printf("Rank %d: previous %d, next %d\n", CCommunicate::MPIRank, CCommunicate::MPIPreviousRank, CCommunicate::MPINextRank);

  int local, global, *pShared, shared, result;

  local = 0;
  global = 0;

  // MPI_Alloc_mem(sizeof(int), MPI_INFO_NULL, &pShared);

  CCommunicate::getRMAIndex();
  CCommunicate::allocateRMA();

  if (CCommunicate::MPIRank == 0)
    {
      CCommunicate::updateRMA(0, &CValueInterface::equal, 0.0);
    }

  MPI_Win_fence(0, CCommunicate::MPIWin);
  global = CCommunicate::getRMA(0);
  MPI_Win_fence(0, CCommunicate::MPIWin);

  printf("Rank %d has new gobal value: %d\n", CCommunicate::MPIRank, global);

  for (i = 0; i < 1000 && global < 100; ++i)
    {
      // std::this_thread::sleep_for(std::chrono::microseconds(uniform(R)));

      if (uniform(R) < 20)
        {
          global = CCommunicate::getRMA(0);

          if (global < 100)
            {
              CCommunicate::updateRMA(0, &CValueInterface::plus, 1.0);
              local++;
            }
        }
    }

  MPI_Win_fence(0, CCommunicate::MPIWin);
  global = CCommunicate::getRMA(0);
  MPI_Win_fence(0, CCommunicate::MPIWin);

  printf("Rank %d has final gobal value: %d\n", CCommunicate::MPIRank, global);
  printf("Rank %d has final local value: %d\n", CCommunicate::MPIRank, local);


  CCommunicate::finalize();

  return 0;
}
