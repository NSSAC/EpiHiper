// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
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
 * CDBDefinition.h
 *
 *  Created on: Aug 8, 2019
 *      Author: shoops
 */

#ifndef SRC_DB_CSCHEMA_H_
#define SRC_DB_CSCHEMA_H_

#include <map>
#include "db/CTable.h"

class CSchema
{
public:
  static CSchema INSTANCE;

  static void load(const std::vector< std::string > & schemaFiles);

  CSchema();

  CSchema(const CSchema & src);

  virtual ~CSchema();
  
  virtual void fromJSON(const json_t * json);

  const std::string & getId() const;

  const bool & isValid() const;

  const CTable & getTable(const std::string & table) const;

  const std::string & getTableForField(const std::string & field) const;

private:
  bool addTable(const json_t * json);

  std::string mId;
  std::string mLabel;
  std::map< std::string, CTable > mTables;
  std::map< std::string, std::string > mFieldToTable;
  bool mValid;
};

#endif /* SRC_DB_CSCHEMA_H_ */
