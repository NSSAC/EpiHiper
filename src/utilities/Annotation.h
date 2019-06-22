// BEGIN: Copyright 
// Copyright (C) 2019 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

/*
 * Annotation.h
 *
 *  Created on: Jun 22, 2019
 *      Author: shoops
 */

#ifndef SRC_UTILITIES_ANNOTATION_H_
#define SRC_UTILITIES_ANNOTATION_H_

#include <string>

struct json_t;

class Annotation
{
  public:
    Annotation();

    Annotation(const Annotation & src);

    virtual ~Annotation();

    void fromJSON(const json_t * json);

    const std::string & getAnnId() const;

    const std::string & getAnnLabel() const;

    const std::string & getAnnDescription() const;

  protected:
    std::string mAnnId;
    std::string mAnnLabel;
    std::string mAnnDescription;
};

#endif /* SRC_UTILITIES_ANNOTATION_H_ */
