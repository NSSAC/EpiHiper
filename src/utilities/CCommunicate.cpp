// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <cassert>
#include <sstream>

#include "utilities/CLogger.h"
#include "utilities/CCommunicate.h"
#include "utilities/CStreamBuffer.h"

// static
void CCommunicate::resizeReceiveBuffer(int size)
{
  if (size <= ReceiveSize)
    return;

  if (size < 0)
    {
      FatalError(ErrorCode::AllocationError, "");
    }

  if (ReceiveBuffer != NULL)
    {
      delete[] ReceiveBuffer;
      ReceiveSize = 0;
    }

  // Assure we have a valid buffer
  ReceiveSize = std::max(1024, size);

  try
    {
      ReceiveBuffer = new char[ReceiveSize];
    }

  catch (...)
    {
      FatalError(ErrorCode::AllocationError, "");
    }
}

// static
void CCommunicate::init(int argc, char ** argv)
{
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &MPIRank);
  MPI_Comm_size(MPI_COMM_WORLD, &MPIProcesses);

  MPINextRank = (MPIProcesses + MPIRank + 1) % MPIProcesses;
  MPIPreviousRank = (MPIProcesses + MPIRank - 1) % MPIProcesses;

  CLogger::setTask(MPIRank, MPIProcesses);

  MPICommunicator = new MPI_Comm[MPIProcesses];
  int Shift = sizeof(int) * 4;
  MPI_Comm Dummy;

  for (int i = 0; i < MPIProcesses; ++i)
    for (int j = i + 1; j < MPIProcesses; ++j)
      {
        MPI_Comm * pComm = &Dummy;
        int color = MPI_UNDEFINED;

        if (i == MPIRank)
          {
            color = (std::min(MPIRank, j) << Shift) + std::max(MPIRank, j);
            pComm = &MPICommunicator[j];
          }
        else if (j == MPIRank)
          {
            color = (std::min(MPIRank, i) << Shift) + std::max(MPIRank, i);
            pComm = &MPICommunicator[i];
          }

        int Result = MPI_Comm_split(MPI_COMM_WORLD, color, MPIRank, pComm);

        if (Result != MPI_SUCCESS)
          {
            int Size = 1024;
            char Error[1024];
            MPI_Error_string(Result, Error, &Size);
            CLogger::error() << "CCommunicate::init: " << Error;
          }

        if (color != MPI_UNDEFINED)
          {
            MPI_Group Group;
            MPI_Comm_group(*pComm, &Group);

            int Size;
            MPI_Group_size(Group, & Size);

            if (Size != 2)
              CLogger::error() << "CCommunicate::init: Group Size " << Size;
          }
      }
}

// static
int CCommunicate::abortMessage(ErrorCode err, const std::string & msg, const char * file, int line)
{
  CLogger::error() << "Rank: " << MPIRank << ", " << file << "(" << line << "): " << msg << std::endl;

  return abort(err);
}

// static
int CCommunicate::abort(ErrorCode errorcode)
{
  return MPI_Abort(MPI_COMM_WORLD, (int) errorcode);
}

// static
int CCommunicate::finalize(void)
{
  if (MPIWinSize)
    {
      MPI_Win_free(&MPIWin);
      MPIWinSize = 0;

      if (MPIRank == 0)
        delete[] RMABuffer;
    }

  return MPI_Finalize();
}

// static
int CCommunicate::send(const void * buf,
                       int count,
                       int dest,
                       MPI_Comm comm)
{
  if (MPIProcesses == 1)
    return MPI_SUCCESS;

  CLogger::debug() << "CCommunicate::send: '" << count << "' bytes to '" << dest << "'.";
  return MPI_Send(buf, count, MPI_CHAR, dest, 0, comm);
}

// static
int CCommunicate::receive(void * buf,
                          int count,
                          int source,
                          MPI_Status * status,
                          MPI_Comm comm)
{
  if (MPIProcesses == 1)
    return MPI_SUCCESS;

  CLogger::debug() << "CCommunicate::receive: '" << count << "' bytes from '" << source << "'.";
  return MPI_Recv(buf, count, MPI_CHAR, source, 0, comm, status);
}

// static
int CCommunicate::broadcast(void * buffer,
                            int count,
                            int root)
{
  if (MPIProcesses == 1)
    return MPI_SUCCESS;

  CLogger::debug() << "CCommunicate::broadcast: '" << count << "' bytes from '" << root << "'.";
  return MPI_Bcast(buffer, count, MPI_CHAR, root, MPI_COMM_WORLD);
}

// static
int CCommunicate::broadcastAll(const void * buffer,
                               int count,
                               CCommunicate::ReceiveInterface * pReceive)
{
  ErrorCode Result = ErrorCode::Success;

  int Count = -1;

  for (int sender = 0; sender < MPIProcesses && Result == ErrorCode::Success; ++sender)
    {
      if (sender != MPIRank)
        {
          broadcast(&Count, sizeof(int), sender);

          if (Count > 0)
            {
              resizeReceiveBuffer(Count);
              broadcast(ReceiveBuffer, Count, sender);
            }

          CStreamBuffer Buffer(ReceiveBuffer, Count);
          std::istream is(&Buffer);

          Result = (*pReceive)(is, sender);
        }
      else
        {
          Count = count;
          broadcast(&Count, sizeof(int), sender);

          if (Count > 0)
            {
              resizeReceiveBuffer(Count);
              memcpy(ReceiveBuffer, buffer, Count * sizeof(char));
              // std::cout << Rank << ", " << sender << ": Communicate::broadcast (send: " << Count << ")" << std::endl;
              broadcast(ReceiveBuffer, Count, sender);
            }
        }
    }

  return (int) Result;
}

// static
int CCommunicate::sequential(int firstRank, CCommunicate::SequentialProcessInterface * pSequential)
{
  ErrorCode Result = ErrorCode::Success;
  Status status;
  int signal = 0;

  if (MPIRank == firstRank)
    {
      (*pSequential)();

      signal = 1;

      if (MPINextRank != firstRank)
        {
          send(&signal, sizeof(int), MPINextRank, MPI_COMM_WORLD);
          receive(&signal, sizeof(int), MPIPreviousRank, &status, MPI_COMM_WORLD);
        }
    }
  else
    {
      receive(&signal, sizeof(int), MPIPreviousRank, &status, MPI_COMM_WORLD);

      (*pSequential)();

      send(&signal, sizeof(int), MPINextRank, MPI_COMM_WORLD);
    }

  return (int) Result;
}

// static
int CCommunicate::master(int masterRank,
                         const void * buffer,
                         int countToCenter,
                         int countFromCenter,
                         CCommunicate::ReceiveInterface * pReceive)
{
  ErrorCode Result = ErrorCode::Success;
  Status status;

  resizeReceiveBuffer(std::max(countToCenter, countFromCenter));

  if (MPIRank == masterRank)
    {
      // Receive from all other
      for (int sender = 0; sender < MPIProcesses && Result == ErrorCode::Success; ++sender)
        {
          if (sender != masterRank)
            {
              receive(ReceiveBuffer, countToCenter, sender, &status, MPI_COMM_WORLD);

              CStreamBuffer Buffer(ReceiveBuffer, countToCenter);
              std::istream is(&Buffer);

              Result = (*pReceive)(is, sender);
            }
        }

      CStreamBuffer Buffer(ReceiveBuffer, countFromCenter);
      std::istream is(&Buffer);
      Result = (*pReceive)(is, masterRank);

      memcpy(ReceiveBuffer, buffer, countFromCenter * sizeof(char));
      broadcast(ReceiveBuffer, countFromCenter, masterRank);
    }
  else
    {
      send(buffer, countToCenter, masterRank, MPI_COMM_WORLD);
      broadcast(ReceiveBuffer, countFromCenter, masterRank);

      CStreamBuffer Buffer(ReceiveBuffer, countFromCenter);
      std::istream is(&Buffer);

      Result = (*pReceive)(is, masterRank);
    }

  return (int) Result;
}

// static 
int CCommunicate::RoundRobinRound;

// static 
int CCommunicate::RoundRobinEven;


void CCommunicate::initRoundRobin()
{
  RoundRobinRound = 0;
  RoundRobinEven = true;
}

// static 
CCommunicate::Schedule CCommunicate::nextRoundRobin(int & other)
{
  Schedule result = Schedule::proceed; 
  bool Odd = (MPIProcesses % 2 == 1);
  int Rounds = Odd ? MPIProcesses: MPIProcesses - 1;
  other = -1;

  if (RoundRobinRound == Rounds)
    return Schedule::finished;

  if (MPIRank == Rounds)
    {
      if (Odd)
        result = Schedule::skip;

      if (RoundRobinEven)
        other = RoundRobinRound / 2;
      else
        other = (Rounds + RoundRobinRound) / 2;

      RoundRobinEven = !RoundRobinEven;
    }
  else
    {
      other = RoundRobinRound - MPIRank;

      if (other < 0)
        other += Rounds;

      if (other == MPIRank)
        other = Rounds;

      if (Odd && other == Rounds)
        result = Schedule::skip;
    }

  ++RoundRobinRound;

  return result;
}

// static
int CCommunicate::roundRobinFixed(const void * buffer,
                                  int count,
                                  CCommunicate::ReceiveInterface * pReceive)
{
  ErrorCode Result = ErrorCode::Success;
  Status status;

  int other = -1;

  initRoundRobin();

  while (true)
    {
      switch (nextRoundRobin(other))
        {
        case Schedule::finished:
          return (int) Result;
          break;

        case Schedule::skip:
          continue;
          break;

        case Schedule::proceed:
          CLogger::debug() << "CCommunicate::roundRobinFixed: self: " << MPIRank << ", other: " << other;

          if (other < MPIRank)
            {
              // send
              if (count > 0)
                {
                  send(buffer, count, 0, MPICommunicator[other]);
                }

              // receive
              if (count > 0)
                {
                  resizeReceiveBuffer(count);
                  receive(ReceiveBuffer, count, 0, &status, MPICommunicator[other]);

                  CStreamBuffer Buffer(ReceiveBuffer, count);
                  std::istream is(&Buffer);

                  Result = (*pReceive)(is, other);
                }
            }
          else
            {
              // receive
              if (count > 0)
                {
                  resizeReceiveBuffer(count);
                  receive(ReceiveBuffer, count, 1, &status, MPICommunicator[other]);

                  CStreamBuffer Buffer(ReceiveBuffer, count);
                  std::istream is(&Buffer);

                  Result = (*pReceive)(is, other);
                }

              // send
              if (count > 0)
                {
                  send(buffer, count, 1, MPICommunicator[other]);
                }
            }
        }
    }

  return (int) Result;
}

// static
int CCommunicate::roundRobin(const void * buffer,
                             int count,
                             CCommunicate::ReceiveInterface * pReceive)
{
  ErrorCode Result = ErrorCode::Success;
  Status status;

  int Count;
  int other = -1;

  initRoundRobin();

  while (true)
    {
      switch (nextRoundRobin(other))
        {
        case Schedule::finished:
          return (int) Result;
          break;

        case Schedule::skip:
          continue;
          break;

        case Schedule::proceed:
          CLogger::debug() << "CCommunicate::roundRobin: self: " << MPIRank << ", other: " << other;

          if (other < MPIRank)
            {
              // send
              Count = count;
              send(&Count, sizeof(int), 0, MPICommunicator[other]);

              if (Count > 0)
                {
                  send(buffer, Count, 0, MPICommunicator[other]);
                }

              // receive
              receive(&Count, sizeof(int), 0, &status, MPICommunicator[other]);

              if (Count > 0)
                {
                  resizeReceiveBuffer(Count);
                  receive(ReceiveBuffer, Count, 0, &status, MPICommunicator[other]);

                  CStreamBuffer Buffer(ReceiveBuffer, Count);
                  std::istream is(&Buffer);

                  Result = (*pReceive)(is, other);
                }
            }
          else
            {
              // receive
              receive(&Count, sizeof(int), 1, &status, MPICommunicator[other]);

              if (Count > 0)
                {
                  resizeReceiveBuffer(Count);
                  receive(ReceiveBuffer, Count, 1, &status, MPICommunicator[other]);

                  CStreamBuffer Buffer(ReceiveBuffer, Count);
                  std::istream is(&Buffer);

                  Result = (*pReceive)(is, other);
                }

              // send
              Count = count;
              send(&Count, sizeof(int), 1, MPICommunicator[other]);

              if (Count > 0)
                {
                  send(buffer, Count, 1, MPICommunicator[other]);
                }
            }
        }
    }

  return (int) Result;
}

// static
int CCommunicate::roundRobin(SendInterface * pSend,
                             ReceiveInterface * pReceive)
{
  ErrorCode Result = ErrorCode::Success;
  Status status;

  int Count;
  int other = -1;

  initRoundRobin();

  while (true)
    {
      switch (nextRoundRobin(other))
        {
        case Schedule::finished:
          return (int) Result;
          break;

        case Schedule::skip:
          continue;
          break;

        case Schedule::proceed:
          CLogger::debug() << "CCommunicate::roundRobin: self: " << MPIRank << ", other: " << other;

          if (other < MPIRank)
            {
              // send
              std::ostringstream os;
              Result = (*pSend)(os, other);

              Count = os.str().size();
              send(&Count, sizeof(int), 0, MPICommunicator[other]);

              if (Count > 0)
                {
                  send(os.str().c_str(), Count, 0, MPICommunicator[other]);
                }

              // receive
              receive(&Count, sizeof(int), 0, &status, MPICommunicator[other]);

              if (Count > 0)
                {
                  resizeReceiveBuffer(Count);
                  receive(ReceiveBuffer, Count, 0, &status, MPICommunicator[other]);

                  CStreamBuffer Buffer(ReceiveBuffer, Count);
                  std::istream is(&Buffer);

                  Result = (*pReceive)(is, other);
                }
            }
          else
            {
              // receive
              receive(&Count, sizeof(int), 1, &status, MPICommunicator[other]);

              if (Count > 0)
                {
                  resizeReceiveBuffer(Count);
                  receive(ReceiveBuffer, Count, 1, &status, MPICommunicator[other]);

                  CStreamBuffer Buffer(ReceiveBuffer, Count);
                  std::istream is(&Buffer);

                  Result = (*pReceive)(is, other);
                }

              // send
              std::ostringstream os;
              Result = (*pSend)(os, other);

              Count = os.str().size();
              send(&Count, sizeof(int), 1, MPICommunicator[other]);

              if (Count > 0)
                {
                  send(os.str().c_str(), Count, 1, MPICommunicator[other]);
                }
            }
        }
    }

  return (int) Result;
}

// static
int CCommunicate::allocateRMA()
{
  MPIWinSize = RMAIndex;
  int result = (int) ErrorCode::Success;

  if (MPIWinSize > 0)
    {
      if (MPIRank == 0)
        {
          /* Everyone will retrieve from a buffer on root */
          RMABuffer = new double[MPIWinSize];
          result = MPI_Win_create(RMABuffer, MPIWinSize, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &MPIWin);
        }
      else
        {
          /* Others only retrieve, so these windows can be size 0 */
          result = MPI_Win_create(NULL, 0, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &MPIWin);
        }

      MPI_Win_fence(0, MPIWin);
    }

  return result;
}

// static 
int CCommunicate::barrierRMA()
{
  int result = (int) ErrorCode::Success;

  if (MPIWinSize > 0)
    result = MPI_Win_fence(0, MPIWin);

  return result; 
}

// static
double CCommunicate::getRMA(const int & index)
{
  if (index >= (int) MPIWinSize)
    return std::numeric_limits< double >::quiet_NaN();

  double Value;

  MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, MPIWin);
  MPI_Get(&Value, 1, MPI_DOUBLE, 0, (int) index, 1, MPI_DOUBLE, MPIWin);
  MPI_Win_flush(0, MPIWin);
  MPI_Win_unlock(0, MPIWin);

  return Value;
}

// static
double CCommunicate::updateRMA(const int & index, CCommunicate::Operator pOperator, const double & value)
{
  if (index >= (int) MPIWinSize)
    return std::numeric_limits< double >::quiet_NaN();

  double Value;

  MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, MPIWin);
  MPI_Get(&Value, 1, MPI_DOUBLE, 0, (int) index, 1, MPI_DOUBLE, MPIWin);
  MPI_Win_flush(0, MPIWin);

  (*pOperator)(Value, value);

  MPI_Put(&Value, 1, MPI_DOUBLE, 0, (int) index, 1, MPI_DOUBLE, MPIWin);
  MPI_Win_flush(0, MPIWin);
  MPI_Win_unlock(0, MPIWin);

  return Value;
}

// static
size_t CCommunicate::getRMAIndex()
{
  size_t Index = RMAIndex;
  ++RMAIndex;

  return Index;
}

// static
void CCommunicate::memUsage(const int & tick)
{
  double vm_usage = 0.0;
  double resident_set = 0.0;

  // the two fields we want
  unsigned long vsize;
  long rss;
  {
    std::string ignore;
    std::ifstream ifs("/proc/self/stat", std::ios_base::in);
    ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
      >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
      >> ignore >> ignore >> vsize >> rss;
  }

  long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
  vm_usage = vsize / 1024.0;
  resident_set = rss * page_size_kb;

  CLogger::info() << "Tick: " << tick << "; VM: " << vm_usage << "; RSS: " << resident_set;
}

CCommunicate::~CCommunicate()
{}

CCommunicate::Receive::Receive(CCommunicate::Receive::Type method)
  : CCommunicate::ReceiveInterface()
  , mMethod(method)
{}

// virtual
CCommunicate::Receive::~Receive() {}

// virtual
CCommunicate::ErrorCode CCommunicate::Receive::operator()(std::istream & is, int sender)
{
  // execute member function
  return (*mMethod)(is, sender);
}

CCommunicate::Send::Send(CCommunicate::Send::Type method)
  : CCommunicate::SendInterface()
  , mMethod(method)
{}

// virtual
CCommunicate::Send::~Send()
{}

// override operator "()"
// virtual 
CCommunicate::ErrorCode CCommunicate::Send::operator()(std::ostream & os, int receiver)
{
  // execute member function
  return (*mMethod)(os, receiver);
}

CCommunicate::SequentialProcess::SequentialProcess(CCommunicate::SequentialProcess::Type method)
  : CCommunicate::SequentialProcessInterface()
  , mMethod(method)
{}

// virtual
CCommunicate::SequentialProcess::~SequentialProcess() {}

// virtual
CCommunicate::ErrorCode CCommunicate::SequentialProcess::operator()()
{
  // execute member function
  return (*mMethod)();
}
