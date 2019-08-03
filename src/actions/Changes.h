/*
 * Changes.h
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#ifndef SRC_ACTIONS_CHANGES_H_
#define SRC_ACTIONS_CHANGES_H_

#include <sstream>

#include "utilities/CCommunicate.h"

class CNode;
class CEdge;
class Variable;
class CMetadata;


class Changes
{
public:
  static void record(const CNode & node, const CMetadata & metadata);
  static void record(const CEdge & edge, const CMetadata & metadata);
  static void record(const Variable & variable, const CMetadata & metadata);
  static void clear();
  static size_t size();

  static const std::ostringstream & getNodes();
  static const std::ostringstream & getEdges();

  static void initDefaultOutput();
  static void writeDefaultOutput();
  static CCommunicate::ErrorCode writeDefaultOutputData();
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
