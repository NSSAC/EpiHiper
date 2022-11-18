// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
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

/*
 * Annotation.h
 *
 *  Created on: Jun 22, 2019
 *      Author: shoops
 */

#ifndef SRC_UTILITIES_CANNOTATION_H_
#define SRC_UTILITIES_CANNOTATION_H_

#include <string>

struct json_t;

class CAnnotation
{
  public:
    CAnnotation();

    CAnnotation(const CAnnotation & src);

    virtual ~CAnnotation();

    CAnnotation & operator =(const CAnnotation & rhs);

    virtual void fromJSON(const json_t * json);

    const std::string & getAnnId() const;

    const std::string & getAnnLabel() const;

    const std::string & getAnnDescription() const;

  protected:
    std::string mAnnId;
    std::string mAnnLabel;
    std::string mAnnDescription;
};

#endif /* SRC_UTILITIES_CANNOTATION_H_ */
