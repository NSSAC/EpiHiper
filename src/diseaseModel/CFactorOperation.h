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

#ifndef SRC_DISEASEMODEL_CFACTOROPERATION_H_
#define SRC_DISEASEMODEL_CFACTOROPERATION_H_

struct json_t;

class CFactorOperation
{
public:
  enum struct Type {
    assign,
    multiply,
    divide,
    __NONE
  };

  CFactorOperation();

  CFactorOperation(const CFactorOperation & src);

  virtual ~CFactorOperation();

  virtual void fromJSON(const json_t * json);

  const bool & isValid() const;

  void apply(double & value) const;

  std::string & getJson();

  bool setJson(const std::string & json);

private:
  void normalizeJSON();

  Type mType;
  double mValue;
  bool mValid;
  std::string mNormalizedJSON;
};

#endif /* SRC_DISEASEMODEL_CFACTOROPERATION_H_ */
