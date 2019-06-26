#ifndef SRC_UTILITIES_RANDOM_H_
#define SRC_UTILITIES_RANDOM_H_

#include <random>

class Random
{
public:
  typedef std::mt19937_64 generator_t;
  typedef generator_t::result_type result_t;
  typedef std::uniform_int_distribution< result_t > uniform_int;
  typedef uniform_int::param_type uniform_int_p;
  typedef std::uniform_real_distribution< double > uniform_real;
  typedef uniform_real::param_type uniform_real_p;
  typedef std::normal_distribution< double > normal;
  typedef normal::param_type normal_p;

  static generator_t G;
  static void randomSeed();
  static void seed(result_t seed);
};

#endif /* SRC_UTILITIES_RANDOM_H_ */
