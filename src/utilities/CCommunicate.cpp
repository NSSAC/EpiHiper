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

#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <cassert>
#include <sstream>
#include <chrono>

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
#ifdef USE_MPI
void CCommunicate::init(int argc, char ** argv)
#else
void CCommunicate::init(int /* argc */ , char ** /* argv */)
#endif // USE_MPI
{
  ThreadIndex.init();

#ifdef USE_MPI
  MPI_Init(&argc, &argv);
#endif // USE_MPI

  MPI_Comm_rank(MPI_COMM_WORLD, &MPIRank);
  MPI_Comm_size(MPI_COMM_WORLD, &MPIProcesses);

  MPINextRank = (MPIProcesses + MPIRank + 1) % MPIProcesses;
  MPIPreviousRank = (MPIProcesses + MPIRank - 1) % MPIProcesses;


  CLogger::setTask(MPIRank, MPIProcesses);

#ifdef USE_MPI
  MPICommunicator = new MPI_Comm[MPIProcesses];
  int Shift = sizeof(int) * 4;
  MPI_Comm Dummy;

  // Initialize pairwise communicators for round robin tournament messaging
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
  #endif // USE_MPI
}

// static 
int CCommunicate::LocalThreadIndex()
{
  return ThreadIndex.localIndex(&ThreadIndex.Active());
}

// static 
int CCommunicate::GlobalThreadIndex()
{
  return ThreadIndex.globalIndex(&ThreadIndex.Active());
}

// static 
int CCommunicate::LocalProcesses()
{
  return ThreadIndex.size();
}

// static 
int CCommunicate::TotalProcesses()
{
  return MPIProcesses * LocalProcesses();
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
#ifdef USE_MPI  
  return MPI_Abort(MPI_COMM_WORLD, (int) errorcode);
#else
  return (int) errorcode;
#endif // USE_MPI
}

// static
int CCommunicate::finalize(void)
{
#ifdef USE_MPI  
  if (MPIWinSize)
    {
      MPI_Win_free(&MPIWin);
      MPIWinSize = 0;

      if (MPIRank == 0)
        delete[] RMABuffer;
    }

  return MPI_Finalize();
#else
  return MPI_SUCCESS;
#endif // USE_MPI
}

// static
#ifdef USE_MPI  
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
#else
int CCommunicate::send(const void * /* buf */,
                       int /* count */,
                       int /* dest */,
                       MPI_Comm /* comm */)
{
  return MPI_SUCCESS;
}
#endif // USE_MPI

// static
#ifdef USE_MPI  
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
#else
int CCommunicate::receive(void * /* buf */,
                          int /* count */,
                          int /* source */,
                          MPI_Status * /* status */,
                          MPI_Comm /* comm */)
{
  return MPI_SUCCESS;
}
#endif // USE_MPI

// static
#ifdef USE_MPI  
int CCommunicate::broadcast(void * buffer,
                            int count,
                            int root)
{
  if (MPIProcesses == 1)
    return MPI_SUCCESS;

  CLogger::debug() << "CCommunicate::broadcast: '" << count << "' bytes from '" << root << "'.";
  return MPI_Bcast(buffer, count, MPI_CHAR, root, MPI_COMM_WORLD);
}
#else
int CCommunicate::broadcast(void * /* buffer */,
                            int /* count */,
                            int /* root */)
{
  return MPI_SUCCESS;
}
#endif // USE_MPI

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
  std::chrono::time_point<std::chrono::steady_clock> Start = std::chrono::steady_clock::now();

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

  CLogger::debug() << "CCommunicate::sequential: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";

  return (int) Result;
}

// static
int CCommunicate::master(int masterRank,
                         const void * buffer,
                         int countToCenter,
                         int countFromCenter,
                         CCommunicate::ReceiveInterface * pReceive)
{
  std::chrono::time_point<std::chrono::steady_clock> Start = std::chrono::steady_clock::now();
 
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

  CLogger::debug() << "CCommunicate::master: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";

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
  std::chrono::time_point<std::chrono::steady_clock> Start = std::chrono::steady_clock::now();
 
  ErrorCode Result = ErrorCode::Success;
  Status status;

  int other = -1;

  initRoundRobin();

  bool Proceed = true;

  while (Proceed)
    {
      switch (nextRoundRobin(other))
        {
        case Schedule::finished:
          Proceed = false;
          break;

        case Schedule::skip:
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

  CLogger::debug() << "CCommunicate::roundRobinFixed: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";

  return (int) Result;
}

// static
int CCommunicate::roundRobin(const void * buffer,
                             int count,
                             CCommunicate::ReceiveInterface * pReceive)
{
  std::chrono::time_point<std::chrono::steady_clock> Start = std::chrono::steady_clock::now();
 
  ErrorCode Result = ErrorCode::Success;
  Status status;

  int Count;
  int other = -1;

  initRoundRobin();

  bool Proceed = true;

  while (Proceed)
    {
      switch (nextRoundRobin(other))
        {
        case Schedule::finished:
          Proceed = false;
          break;

        case Schedule::skip:
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

  CLogger::debug() << "CCommunicate::roundRobin: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";

  return (int) Result;
}

// static
int CCommunicate::roundRobin(SendInterface * pSend,
                             ReceiveInterface * pReceive)
{
  std::chrono::time_point<std::chrono::steady_clock> Start = std::chrono::steady_clock::now();

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

  CLogger::debug() << "CCommunicate::roundRobin: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";

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
#ifdef USE_MPI
          result = MPI_Win_create(RMABuffer, MPIWinSize, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &MPIWin);
#endif // USE_MPI
        }
      else
        {
          /* Others only retrieve, so these windows can be size 0 */
#ifdef USE_MPI
          result = MPI_Win_create(NULL, 0, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &MPIWin);
#endif // USE_MPI
        }

      result = barrierRMA();
    }

  return result;
}

// static 
int CCommunicate::barrierRMA()
{
  int result = (int) ErrorCode::Success;

#pragma omp barrier

#ifdef USE_MPI
  if (MPIWinSize > 0)
    {
      std::chrono::time_point<std::chrono::steady_clock> Start = std::chrono::steady_clock::now();

#pragma omp single
      result = MPI_Win_fence(0, MPIWin);

      CLogger::debug() << "CCommunicate::barrierRMA: duration = '" << std::chrono::nanoseconds(std::chrono::steady_clock::now() - Start).count()/1000  << "' ms.";
    }
#endif // USE_MPI

  return result; 
}

// static
double CCommunicate::getRMA(const int & index)
{
  double Value = std::numeric_limits< double >::quiet_NaN();

  if (index < (int) MPIWinSize)
#pragma omp critical
    {
#ifdef USE_MPI
      MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, MPIWin);
      MPI_Get(&Value, 1, MPI_DOUBLE, 0, (int) index, 1, MPI_DOUBLE, MPIWin);
      MPI_Win_flush(0, MPIWin);
      MPI_Win_unlock(0, MPIWin);
#endif // USE_MPI
    }

  return Value;
}

// static
double CCommunicate::updateRMA(const int & index, CCommunicate::Operator pOperator, const double & value)
{
  double Value = std::numeric_limits< double >::quiet_NaN();

  if (index < (int) MPIWinSize)
#pragma omp critical
    {
#ifdef USE_MPI
      MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, MPIWin);
      MPI_Get(&Value, 1, MPI_DOUBLE, 0, (int) index, 1, MPI_DOUBLE, MPIWin);
      MPI_Win_flush(0, MPIWin);
#endif // USE_MPI

      (*pOperator)(Value, value);

#ifdef USE_MPI
      MPI_Put(&Value, 1, MPI_DOUBLE, 0, (int) index, 1, MPI_DOUBLE, MPIWin);
      MPI_Win_flush(0, MPIWin);
      MPI_Win_unlock(0, MPIWin);
#endif // USE_MPI
    }
  
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

  CLogger::info() << "Tick: " << tick + 1 << "; VM: " << (size_t) vm_usage << "; RSS: " << (size_t) resident_set;
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
