#include <sstream>
#include "Misc.h"

std::string trim(const std::string& s, const std::string& r) {
  std::size_t foundStart = s.find_first_not_of(r);
  if (foundStart == std::string::npos) return "";
  std::size_t foundEnd = s.find_last_not_of(r);
  return s.substr(foundStart, foundEnd-foundStart+1);
}

std::vector<std::string> split(const std::string& s, char c) {
  std::vector<std::string> items;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, c)) {
    items.push_back(item);
  }
  return items;
}

int myRandom(int i) {
  return lrand48() % i;
}
