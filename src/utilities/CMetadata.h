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

/*
 * MetaData.h
 *
 *  Created on: Jul 11, 2019
 *      Author: shoops
 */

#ifndef SRC_UTILITIES_CMETADATA_H_
#define SRC_UTILITIES_CMETADATA_H_

#include <string>

struct json_t;

class CMetadata
{
public:
  enum struct Type
  {
    Object,
    Array,
    Boolean,
    String,
    Integer,
    Double,
    Null,
    NotFound
  };

  CMetadata();
  CMetadata(const std::string & key, const std::string & value);
  CMetadata(const std::string & key, const bool & value);
  CMetadata(const std::string & key, const double & value);
  CMetadata(const std::string & key, const int & value);
  CMetadata(const std::string & key, const CMetadata & value);
  CMetadata(const CMetadata & src);

  virtual ~CMetadata();

  void set(const std::string & key, const std::string & value);
  void set(const std::string & key, const bool & value);
  void set(const std::string & key, const double & value);
  void set(const std::string & key, const int & value);
  void set(const std::string & key, const CMetadata & value);

  bool contains(const std::string & key) const;
  Type getType(const std::string & key) const;
  std::string getString(const std::string & key) const;
  bool getBool(const std::string & key) const;
  double getDouble(const std::string & key) const;
  int getInt(const std::string & key) const;
  CMetadata getObject(const std::string & key) const;

  void toBinary(std::ostream & os) const;
  bool fromBinary(std::istream & is);

private:
  CMetadata(json_t * pObject);
  void set(const std::string & key, json_t * pValue);
  bool get(const std::string & key, json_t *& pValue) const;

  json_t * mpJson;
};


#endif /* SRC_UTILITIES_CMETADATA_H_ */
