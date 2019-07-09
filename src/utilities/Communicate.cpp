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

#include "Communicate.h"

// static
int Communicate::Rank(-1);

// static
int Communicate::Processes(-1);

// static
int Communicate::ReceiveSize(0);

// static
char * Communicate::ReceiveBuffer(NULL);

// static
void Communicate::resizeReceiveBuffer(int size)
{
  if (size <= ReceiveSize) return;

  if (size < 0)
    {
      abort(ErrorCode::AllocationError);
    }

  if (ReceiveBuffer != NULL)
    {
      delete [] ReceiveBuffer;
      ReceiveSize = 0;
    }

  // Assure we have a valid buffer
  ReceiveSize = std::min(1024, size);

  try {
      ReceiveBuffer = new char[ReceiveSize];
  }

  catch (...) {
      abort(ErrorCode::AllocationError);
  }
}

// static
void Communicate::init(int *argc, char ***argv)
{
  MPI_Init(argc, argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
  MPI_Comm_size(MPI_COMM_WORLD, &Processes);
}

// static
int Communicate::abort(ErrorCode errorcode)
{
  return MPI_Abort(MPI_COMM_WORLD, (int) errorcode);
}

// static
int Communicate::finalize(void)
{
  return  MPI_Finalize();
}

// static
int Communicate::send(const void *buf,
                      int count,
                      MPI_Datatype datatype,
                      int dest,
                      int tag)
{
  return MPI_Send(buf, count, datatype, dest, tag, MPI_COMM_WORLD);
}

// static
int Communicate::receive(void *buf,
                         int count,
                         MPI_Datatype  datatype,
                         int source,
                         int tag,
                         MPI_Status *status)
{
  return MPI_Recv(buf, count, datatype, source, tag, MPI_COMM_WORLD, status);
}

// static
int Communicate::broadcast(void *buffer,
                           int count,
                           MPI_Datatype datatype,
                           int root)
{
  return MPI_Bcast(buffer, count, datatype, root, MPI_COMM_WORLD);
}

// static
int Communicate::broadcast(void *buffer,
                           int count,
                           Communicate::ReceiveInterface * pReceive)
{
  ErrorCode Result = ErrorCode::Success;
  int rCount = -1;

  for (int sender = 0; sender < Processes && Result == ErrorCode::Success; ++sender)
    {
      if (sender != Rank)
        {
          broadcast(&rCount, 1, MPI_INT, sender);
          resizeReceiveBuffer(rCount);

          broadcast(ReceiveBuffer, rCount, MPI_CHAR, sender);
          Result = (*pReceive)(ReceiveBuffer, rCount, sender);
        }
      else
        {
          broadcast(&count, 1, MPI_INT, Rank);
          broadcast(buffer, count, MPI_CHAR, Rank);
        }
    }

  return (int) Result;
}

// static
int Communicate::sequential(int firstRank, Communicate::SequentialProcessInterface * pSequential)
{
  ErrorCode Result = ErrorCode::Success;
  Status status;
  int signal = 0;

  if (Rank == firstRank)
    {
      (*pSequential)();

      signal = 1;
      Communicate::send(&signal, 1, MPI_INT, (firstRank + 1)%Communicate::Processes, firstRank);
      Communicate::receive(&signal, 1, MPI_INT, (firstRank - 1)%Communicate::Processes, (firstRank - 1)%Communicate::Processes, &status);
    }
  else
    {
      Communicate::receive(&signal, 1, MPI_INT, Communicate::Rank-1, Communicate::Rank-1,  &status);

      (*pSequential)();

      Communicate::send(&signal, 1, MPI_INT, (Communicate::Rank+1)%Communicate::Processes, Communicate::Rank);
    }

  return (int) Result;
}

Communicate::~Communicate()
{}

Communicate::Receive::Receive(Communicate::Receive::Type method)
  : Communicate::ReceiveInterface()
  , mMethod(method)
{}

// virtual
Communicate::Receive::~Receive() {}

// virtual
Communicate::ErrorCode Communicate::Receive::operator()(void * rBuffer, int rCount, int sender)
{
  // execute member function
  return (*mMethod)(rBuffer, rCount, sender);
}

Communicate::SequentialProcess::SequentialProcess(Communicate::SequentialProcess::Type method)
  : Communicate::SequentialProcessInterface()
  , mMethod(method)
{}

// virtual
Communicate::SequentialProcess::~SequentialProcess() {}

// virtual
Communicate::ErrorCode Communicate::SequentialProcess::operator()()
{
  // execute member function
  return (*mMethod)();
}


