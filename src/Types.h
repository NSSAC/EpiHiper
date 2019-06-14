#ifndef EPIHIPER_TYPES_H
#define EPIHIPER_TYPES_H

#include <cstdint>
#include <string>

typedef uint64_t personid_t;
typedef uint64_t netsize_t;
struct health_t {
  std::string id;
  std::string label;
  float susceptibility;
  float infectivity;
};
struct infection_t {
  std::string id;
  std::string label;
  health_t entryState;
  health_t exitState;
  health_t contactState;
  float transmissibility;
};
struct transition_t {
  std::string id;
  std::string label;
  health_t entryState;
  health_t exitState;
  float prob;
  // Distribution dwellTime; // TODO: class Distribution
};

#endif
