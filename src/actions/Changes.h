/*
 * Changes.h
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#ifndef SRC_ACTIONS_CHANGES_H_
#define SRC_ACTIONS_CHANGES_H_

#include <sstream>

#include "utilities/Communicate.h"

class Node;
class Edge;
class Variable;
class Metadata;


class Changes
{
public:
  static void record(const Node & node, const Metadata & metadata);
  static void record(const Edge & edge, const Metadata & metadata);
  static void record(const Variable & variable, const Metadata & metadata);
  static void clear();
  static size_t size();

  static const std::ostringstream & getNodes();
  static const std::ostringstream & getEdges();

  static void initDefaultOutput();
  static void writeDefaultOutput();
  static Communicate::ErrorCode writeDefaultOutputData();
  static void setCurrentTick(size_t tick);
  static void incrementTick();

  Changes() = delete;
  virtual ~Changes();

private:
  static std::ostringstream Nodes;
  static std::ostringstream Edges;
  static std::stringstream DefaultOutput;

  static size_t Size;
  static size_t Tick;
};

#endif /* SRC_ACTIONS_CHANGES_H_ */
