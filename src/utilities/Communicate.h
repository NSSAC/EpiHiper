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

    static int abort(int errorcode);

    static int finalize(void);

    Communicate() = delete;
    virtual ~Communicate();
};

#endif /* SRC_COMMUNICATE_H_ */
