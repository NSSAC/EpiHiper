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
 * CConnection.cpp
 *
 *  Created on: Aug 8, 2019
 *      Author: shoops
 */

#include "db/CConnection.h"
#include "SimConfig.h"

// static
CConnection * CConnection::pINSTANCE = NULL;

// static
void CConnection::init()
{
  if (pINSTANCE != NULL) return;

  const SimConfig::db_connection & dbConnection = SimConfig::getDBConnection();

  // postgresql://[user[:password]@][netloc][:port][,...][/dbname][?param1=value1&...]
  std::string URI = "postgresql://" + dbConnection.user + ":" + dbConnection.password + "@" + dbConnection.host + "/" + dbConnection.name;

  pINSTANCE = new CConnection(URI);
}

// static
pqxx::work * CConnection::work()
{
  init();

  return new pqxx::work(pINSTANCE->mConnection);
}

CConnection::CConnection(const std::string & uri)
  : mConnection(uri)
{}

CConnection::~CConnection()
{}

