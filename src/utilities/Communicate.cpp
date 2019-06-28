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

/*
 * Communicate.cpp
 *
 *  Created on: Jun 27, 2019
 *      Author: shoops
 */

#include "Communicate.h"

// static
int Communicate::Rank(-1);

// static
int Communicate::Processes(-1);

// static
void Communicate::init(int *argc, char ***argv)
{
  MPI_Init(argc, argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
  MPI_Comm_size(MPI_COMM_WORLD, &Processes);
}

// static
int Communicate::abort(int errorcode)
{
  return MPI_Abort(MPI_COMM_WORLD, errorcode);
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

Communicate::~Communicate()
{}

