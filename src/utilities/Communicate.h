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

#ifndef SRC_COMMUNICATE_H_
#define SRC_COMMUNICATE_H_

#include <mpi.h>

class Communicate
{
public:
  enum struct ErrorCode {
    Success = MPI_SUCCESS,
    // space for MPI_ERR_... see mpi.h
    InvalidArguments = MPI_ERR_LASTCODE + 1,
    AllocationError,
    FileOpenError
  };

  class ReceiveInterface
  {
  public:
    typedef ErrorCode (*Type)(void * /* rBuffer */ , int /* rCount */ , int /* sender */);

    virtual ~ReceiveInterface() {};

    virtual ErrorCode operator()(void * /* rBuffer */ , int /* rCount */ , int /* sender */) = 0;
  };

  class Receive : public ReceiveInterface
  {
  public:
    Receive() = delete;
    Receive(Type method);

    virtual ~Receive();

    // override operator "()"
    virtual ErrorCode operator()(void * rBuffer, int rCount, int sender);

  private:
    Type mMethod;
  };

  template <class Receiver>
  class ClassMemberReceive : public ReceiveInterface
  {
  public:
    ClassMemberReceive() = delete;

    ClassMemberReceive(Receiver * pReceiver,
                       ErrorCode (Receiver::*method)(void * /* rBuffer */ , int /* rCount */ , int /* sender */));

    virtual ~ClassMemberReceive();

    // override operator "()"
    virtual ErrorCode operator()(void * rBuffer, int rCount, int sender);

  private:
    /**
     * The pointer to the instance of the caller
     */
    Receiver * mpReceiver;             // pointer to object
    ErrorCode (Receiver::*mpMethod)(void * /* rBuffer */ , int /* rCount */ , int /* sender */);
  };

  class SequentialProcessInterface
  {
  public:
    typedef ErrorCode (*Type)();

    virtual ~SequentialProcessInterface() {};

    virtual ErrorCode operator()() = 0;
  };

  class SequentialProcess : public SequentialProcessInterface
  {
  public:
    SequentialProcess() = delete;
    SequentialProcess(Type method);

    virtual ~SequentialProcess();

    // override operator "()"
    virtual ErrorCode operator()();

  private:
    Type mMethod;
  };

  template <class Processor>
  class ClassMemberSequentialProcess : public SequentialProcessInterface
  {
  public:
    ClassMemberSequentialProcess() = delete;

    ClassMemberSequentialProcess(Processor * pProcessor,
                                 ErrorCode (Processor::*method)());

    virtual ~ClassMemberSequentialProcess();

    // override operator "()"
    virtual ErrorCode operator()();

  private:
    /**
     * The pointer to the instance of the caller
     */
    Processor * mpProcessor;             // pointer to object
    ErrorCode (Processor::*mpMethod)();
  };

  typedef MPI_Status Status;

  static int Rank;
  static int Processes;
  static void init(int *argc, char ***argv);

  static int send(const void *buf,
                  int count,
                  MPI_Datatype datatype,
                  int dest,
                  int tag);

  static int receive(void *buf,
                     int count,
                     MPI_Datatype datatype,
                     int source,
                     int tag,
                     MPI_Status *status);

  static int broadcast(void *buffer,
                       int count,
                       MPI_Datatype datatype,
                       int root);

  static int broadcast(void *buffer,
                       int count,
                       ReceiveInterface * pReceive);

  static int sequential(int firstRank, SequentialProcessInterface * pSequential);

  static int abort(ErrorCode errorcode);

  static int finalize(void);

  Communicate() = delete;
  virtual ~Communicate();

  private:
  static int ReceiveSize;
  static char * ReceiveBuffer;

  static void resizeReceiveBuffer(int size);
};

template <class Receiver>
Communicate::ClassMemberReceive< Receiver >::ClassMemberReceive(Receiver * pReceiver,
                                                                ErrorCode (Receiver::*method)(void * /* rBuffer */ , int /* rCount */ , int /* sender */)):
                                                                ReceiveInterface(),
                                                                mpReceiver(pReceiver),
                                                                mpMethod(method){}

// virtual
template <class Receiver>
Communicate::ClassMemberReceive< Receiver >::~ClassMemberReceive() {}

// override operator "()"
// virtual
template <class Receiver>
Communicate::ErrorCode Communicate::ClassMemberReceive< Receiver >::operator()(void * rBuffer, int rCount, int sender)
{
  // execute member function
  return (*mpReceiver.*mpMethod)(rBuffer, rCount, sender);
}

template <class Processor>
Communicate::ClassMemberSequentialProcess< Processor >::ClassMemberSequentialProcess(Processor * pProcessor,
                                                                                     ErrorCode (Processor::*method)()):
                                                                                     SequentialProcessInterface(),
                                                                                     mpProcessor(pProcessor),
                                                                                     mpMethod(method){}

// virtual
template <class Processor>
Communicate::ClassMemberSequentialProcess< Processor >::~ClassMemberSequentialProcess() {}

// override operator "()"
// virtual
template <class Processor>
Communicate::ErrorCode Communicate::ClassMemberSequentialProcess< Processor >::operator()()
{
  // execute member function
  return (*mpProcessor.*mpMethod)();
}

#endif /* SRC_COMMUNICATE_H_ */
