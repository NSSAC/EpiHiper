// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
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

#ifndef SRC_COMMUNICATE_H_
#define SRC_COMMUNICATE_H_

#include <iostream>

#include "utilities/CContext.h"

#define FatalError(err, msg) \
  { \
    CCommunicate::abortMessage((err), (msg), __FILE__, __LINE__); \
  }

class COperation;

class CCommunicate
{
public:
  typedef void (*Operator)(double &, const double &);

  enum struct ErrorCode
  {
    Success = MPI_SUCCESS,
    // space for MPI_ERR_... see mpi.h
    InvalidArguments = MPI_ERR_LASTCODE + 1,
    AllocationError,
    FileOpenError,
    TypeMissmatch,
    InvalidOperation
  };

  class ReceiveInterface
  {
  public:
    typedef ErrorCode (*Type)(std::istream & /* is */, int /* sender */);

    virtual ~ReceiveInterface(){};

    virtual ErrorCode operator()(std::istream & /* is */, int /* sender */) = 0;
  };

  class Receive : public ReceiveInterface
  {
  public:
    Receive() = delete;
    Receive(Type method);

    virtual ~Receive();

    // override operator "()"
    virtual ErrorCode operator()(std::istream & is, int sender) override;

  private:
    Type mMethod;
  };

  template < class Receiver >
  class ClassMemberReceive : public ReceiveInterface
  {
  public:
    ClassMemberReceive() = delete;

    ClassMemberReceive(Receiver * pReceiver,
                       ErrorCode (Receiver::*method)(std::istream & /* is */, int /* sender */));

    virtual ~ClassMemberReceive();

    // override operator "()"
    virtual ErrorCode operator()(std::istream & is, int sender) override;

  private:
    /**
     * The pointer to the instance of the caller
     */
    Receiver * mpReceiver; // pointer to object
    ErrorCode (Receiver::*mpMethod)(std::istream & /* is */, int /* sender */);
  };

  class SendInterface
  {
  public:
    typedef ErrorCode (*Type)(std::ostream & /* os */, int /* receiver */);

    virtual ~SendInterface(){};

    virtual ErrorCode operator()(std::ostream & /* os */, int /* receiver */) = 0;
  };

  class Send : public SendInterface
  {
  public:
    Send() = delete;
    Send(Type method);

    virtual ~Send();

    // override operator "()"
    virtual ErrorCode operator()(std::ostream & os, int receiver) override;

  private:
    Type mMethod;
  };

  template < class Sender >
  class ClassMemberSend : public SendInterface
  {
  public:
    ClassMemberSend() = delete;

    ClassMemberSend(Sender * pSender,
                    ErrorCode (Sender::*method)(std::ostream & /* os */, int /* receiver */));

    virtual ~ClassMemberSend();

    // override operator "()"
    virtual ErrorCode operator()(std::istream & os, int receiver) override;

  private:
    /**
     * The pointer to the instance of the caller
     */
    Sender * mpSender; // pointer to object
    ErrorCode (Sender::*mpMethod)(std::ostream & /* os */, int /* receiver */);
  };

  class SequentialProcessInterface
  {
  public:
    typedef ErrorCode (*Type)();

    virtual ~SequentialProcessInterface(){};

    virtual ErrorCode operator()() = 0;
  };

  class SequentialProcess : public SequentialProcessInterface
  {
  public:
    SequentialProcess() = delete;
    SequentialProcess(Type method);

    virtual ~SequentialProcess();

    // override operator "()"
    virtual ErrorCode operator()() override;

  private:
    Type mMethod;
  };

  template < class Processor >
  class ClassMemberSequentialProcess : public SequentialProcessInterface
  {
  public:
    ClassMemberSequentialProcess() = delete;

    ClassMemberSequentialProcess(Processor * pProcessor,
                                 ErrorCode (Processor::*method)());

    virtual ~ClassMemberSequentialProcess();

    // override operator "()"
    virtual ErrorCode operator()() override;

  private:
    /**
     * The pointer to the instance of the caller
     */
    Processor * mpProcessor; // pointer to object
    ErrorCode (Processor::*mpMethod)();
  };

  typedef MPI_Status Status;

private:
  static CContext< size_t > ThreadIndex;

  static int send(const void * buf,
                  int count,
                  int dest,
                  MPI_Comm comm);

  static int receive(void * buf,
                     int count,
                     int source,
                     MPI_Status * status,
                     MPI_Comm comm);

  static int broadcastAll(const void * buffer,
                          int count,
                          ReceiveInterface * pReceive);

public:
  static int MPIRank;
  static int MPINextRank;
  static int MPIPreviousRank;
  static int MPIProcesses;
  static MPI_Comm * MPICommunicator;

  static void init(int argc, char ** argv);

  static int LocalThreadIndex();

  static int GlobalThreadIndex();

  static int LocalProcesses();

  static int TotalProcesses();
  
  static int broadcast(void * buffer,
                       int count,
                       int root);

  static int sequential(int firstRank, SequentialProcessInterface * pSequential);

  static int master(int centerRank,
                    const void * buffer,
                    int countIn,
                    int countOut,
                    CCommunicate::ReceiveInterface * pReceive);

  static int roundRobinFixed(const void * buffer,
                             int count,
                             ReceiveInterface * pReceive);

  static int roundRobin(const void * buffer,
                        int count,
                        ReceiveInterface * pReceive);

  static int roundRobin(SendInterface * pSend,
                        ReceiveInterface * pReceive);

  static int abortMessage(ErrorCode err, const std::string & msg, const char * file, int line);

  static int abort(ErrorCode errorcode);

  static int finalize(void);

  static int allocateRMA();

  static int barrierRMA();

  static double getRMA(const int & index);

  static double updateRMA(const int & index, Operator pOperator, const double & value);

  static size_t getRMAIndex();

  static void memUsage(const int & tick);

  CCommunicate() = delete;
  virtual ~CCommunicate();

  static MPI_Win MPIWin;

private:
  static int ReceiveSize;
  static char * ReceiveBuffer;
  static size_t MPIWinSize;
  static double * RMABuffer;
  static size_t RMAIndex;

  static void resizeReceiveBuffer(int size);

 // static
  enum struct Schedule
  {
    proceed,
    finished,
    skip
  };

  static int RoundRobinRound;
  static int RoundRobinEven;

  static void initRoundRobin();
  static Schedule nextRoundRobin(int & other);
};

template < class Receiver >
CCommunicate::ClassMemberReceive< Receiver >::ClassMemberReceive(Receiver * pReceiver,
                                                                 ErrorCode (Receiver::*method)(std::istream & /* is */, int /* sender */))
  : ReceiveInterface()
  , mpReceiver(pReceiver)
  , mpMethod(method)
{}

// virtual
template < class Receiver >
CCommunicate::ClassMemberReceive< Receiver >::~ClassMemberReceive()
{}

// override operator "()"
// virtual
template < class Receiver >
CCommunicate::ErrorCode CCommunicate::ClassMemberReceive< Receiver >::operator()(std::istream & is, int sender)
{
  // execute member function
  return (*mpReceiver.*mpMethod)(is, sender);
} 

template < class Processor >
CCommunicate::ClassMemberSequentialProcess< Processor >::ClassMemberSequentialProcess(Processor * pProcessor,
                                                                                      ErrorCode (Processor::*method)())
  : SequentialProcessInterface()
  , mpProcessor(pProcessor)
  , mpMethod(method)
{}

// virtual
template < class Processor >
CCommunicate::ClassMemberSequentialProcess< Processor >::~ClassMemberSequentialProcess()
{}

// override operator "()"
// virtual
template < class Processor >
CCommunicate::ErrorCode CCommunicate::ClassMemberSequentialProcess< Processor >::operator()()
{
  // execute member function
  return (*mpProcessor.*mpMethod)();
}

#endif /* SRC_COMMUNICATE_H_ */
