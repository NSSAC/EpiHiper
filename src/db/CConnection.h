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

#ifndef SRC_DB_CCONNECTION_H_
#define SRC_DB_CCONNECTION_H_

#ifndef PQXX_CONNECTION
#define PQXX_CONNECTION void
#endif /* PQXX_CONNECTION */

class CConnection
{
public:
  CConnection();
  virtual ~CConnection();

private:
  PQXX_CONNECTION * mpConnection;
};

#endif /* SRC_DB_CCONNECTION_H_ */
