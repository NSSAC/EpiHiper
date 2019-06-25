#include <mpi.h>

#include "Random.h"

// static
Random::generator_t Random::G;

// static
void Random::randomSeed()
{
  std::random_device rd;
  seed(rd());
}

// static
void Random::seed(Random::result_t value)
{
  uint64_t Seed = value;
  int myRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  MPI_Bcast(&Seed, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

  Random::G.seed(Seed + myRank);
}
