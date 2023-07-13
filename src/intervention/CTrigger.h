// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
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

#ifndef SRC_INTERVENTION_CTRIGGER_H_
#define SRC_INTERVENTION_CTRIGGER_H_

#include <vector>
#include <map>

#include "math/CComputable.h"
#include "utilities/CAnnotation.h"
#include "utilities/CCommunicate.h"
#include "actions/CConditionDefinition.h"

class CIntervention;
struct json_t;

class CTrigger : public CAnnotation
{
public:
  static std::vector< CTrigger * > INSTANCES;

  static bool * pGlobalTriggered;

  static CComputableSet RequiredTargets;

  static void loadJSON(const json_t * json);

  static void clear();

  static bool processAll();

  static CCommunicate::ErrorCode receive(std::istream & is, int sender);

  CTrigger();

  CTrigger(const CTrigger & src);

  CTrigger(const json_t * json);

  virtual ~CTrigger();

  virtual void process();

  virtual void fromJSON(const json_t * json) override;

  void trigger(const bool & triggers);

  const bool & isValid() const;

private:
  CConditionDefinition mCondition;
  std::map< std::string, CIntervention * > mInterventions;
  bool mIsLocalTrue;
  bool mValid;
};

#endif /* SRC_INTERVENTION_CTRIGGER_H_ */
