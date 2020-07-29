// BEGIN: Copyright 
// Copyright (C) 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <omp.h>
#include <mpi.h>
#include <cstring>

#ifndef UTILITIES_CCONTEXT_H
#define UTILITIES_CCONTEXT_H

template < class Data > class CContext
{
public:
  CContext(); 
  CContext(const CContext & src);
  ~CContext(); 
  CContext & operator = (const CContext & rhs);
  bool operator == (const CContext & rhs) const;

  void init();
  void release();
  Data & Master();
  Data & Active();
  const Data & Master() const;
  const Data & Active() const;
  Data * beginThread();
  Data * endThread();
  const Data * beginThread() const;
  const Data * endThread() const;
  bool isMaster(const Data * data) const;
  bool isThread(const Data * data) const;
  int localIndex(const Data * data) const;
  int globalIndex(const Data * data) const;
  const size_t & size() const;

protected:
  Data * MasterData;
  Data * ThreadData;
  size_t Size;
};

template < class Data > CContext< Data >::CContext()
  : MasterData(NULL)
  , ThreadData(NULL)
  , Size(0)
{}

template < class Data > CContext< Data >::CContext(const CContext & src)
  : MasterData(NULL)
  , ThreadData(NULL)
  , Size(0)
{
  init();

  *MasterData = *src.MasterData;

  Data * pIt = ThreadData;
  Data * pEnd = ThreadData + Size;
  const Data * pSrc = src.ThreadData;

  for (; pIt != pEnd; ++pIt, ++pSrc)
    if (isThread(pIt))
      *pIt = *pSrc;
}

template < class Data > CContext< Data >::~CContext()
{
  release();
}

template < class Data > CContext< Data > & CContext< Data >::operator = (const CContext< Data > & rhs)
{
  if (this != &rhs)
    {
      init();

      *MasterData = *rhs.MasterData;

      Data * pIt = ThreadData;
      Data * pEnd = ThreadData + Size;
      const Data * pRhs = rhs.ThreadData;

      for (; pIt != pEnd; ++pIt, ++pRhs)
        if (isThread(pIt))
          *pIt = *pRhs;
    }

  return *this;
}

template < class Data > bool CContext< Data >::operator == (const CContext< Data > & rhs) const
{
  if (Size != rhs.Size)
   return false;

  if (memcmp(MasterData, rhs.MasterData, sizeof(Data)))
    return false;

  const Data * pIt = ThreadData;
  const Data * pEnd = ThreadData + Size;
  const Data * pRhs = rhs.ThreadData;

  for (; pIt != pEnd; ++pIt, ++pRhs)
    if (isThread(pIt) 
        && memcmp(pIt, pRhs, sizeof(Data)))
      return false;

  return true;
}

template < class Data > void CContext< Data >::init()
{
  if (Size != 0)
    return;
    
  MasterData = new Data();
  Size = omp_get_max_threads();

  if (Size > 1)
    {
      ThreadData = new Data[Size];
    }
  else
    {
      ThreadData = MasterData;
    }
}

template < class Data > void CContext< Data >::release()
{
  if (MasterData != NULL)
    {
      delete MasterData;
      MasterData = NULL;

      if (Size == 1)
        ThreadData = NULL;
    }

  if (ThreadData != NULL)
    {
      delete [] ThreadData;
      ThreadData = NULL;
    }

  Size = 0;
}

template < class Data > Data & CContext< Data >::Master()
{
  return *MasterData;
}

template < class Data > Data & CContext< Data >::Active()
{
  switch (omp_get_num_threads())
    {
      case 1:
        return *MasterData;
        break;

      default:
        return ThreadData[omp_get_thread_num()];
        break;
    }

  return *MasterData;
}

template < class Data > const Data & CContext< Data >::Master() const
{
  return *MasterData;
}

template < class Data > const Data & CContext< Data >::Active() const
{
  switch (omp_get_num_threads())
    {
      case 1:
        return *MasterData;
        break;

      default:
        return ThreadData[omp_get_thread_num()];
        break;
    }

  return *MasterData;
}

template < class Data > Data * CContext< Data >::beginThread()
{
  return ThreadData;
}

template < class Data > Data * CContext< Data >::endThread()
{
  return ThreadData + Size;
}

template < class Data > const Data * CContext< Data >::beginThread() const
{
  return ThreadData;
}

template < class Data > const Data * CContext< Data >::endThread() const
{
  return ThreadData + Size;
}

template < class Data > bool CContext< Data >::isMaster(const Data * data) const
{
  return MasterData == data;
}

template < class Data > bool CContext< Data >::isThread(const Data * data) const
{
  return MasterData != data;
}

template < class Data > int CContext< Data >::localIndex(const Data * data) const
{
  if (ThreadData <= data && data < ThreadData + Size)
    return data - ThreadData;

  return -1;
}

template < class Data > int CContext< Data >::globalIndex(const Data * data) const
{
  int GlobalIndex = isMaster(data) ? 0 : localIndex(data);

  if (GlobalIndex != -1)
    {
      int MPIRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &MPIRank);
      GlobalIndex += MPIRank * Size;
    }

  return GlobalIndex;
}

template < class Data > const size_t & CContext< Data >::size() const
{
  return Size;
}

#endif // UTILITIES_CCONTEXT_H