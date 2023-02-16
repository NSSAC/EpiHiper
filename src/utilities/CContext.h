// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2023 Rector and Visitors of the University of Virginia 
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

#ifndef UTILITIES_CCONTEXT_H
#define UTILITIES_CCONTEXT_H

#include <cstring>
#include "EpiHiperConfig.h"

#ifdef USE_MPI
#include <mpi.h>
#else
  typedef int MPI_Status;
  typedef int MPI_Comm;
  typedef int MPI_Win;
# define MPI_COMM_WORLD 1
# define MPI_SUCCESS 0
# define MPI_ERR_UNKNOWN 2
# define MPI_ERR_LASTCODE 100
# define MPI_Comm_rank(comm, rank) (*rank = 0)
# define MPI_Comm_size(comm, processes) (*processes = 1)
#endif

#ifdef USE_OMP
# include <omp.h>
#else
# define omp_get_max_threads() (1)
# define omp_get_num_threads() (1)
# define omp_get_thread_num() (0)
#endif // USE_OMP

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