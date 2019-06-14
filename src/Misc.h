// miscellaneous helper functions

#ifndef EPIHIPER_MISC_H
#define EPIHIPER_MISC_H

#include <string>
#include <vector>

// remove leading and trailing white spaces (and specified characters)
std::string trim(const std::string& s, const std::string& r);

// split string by given char delimiter
std::vector<std::string> split(const std::string& s, char c);

// choose an integer randomly in [0,i)
int myRandom(int i);

#endif
