// BEGIN: Copyright 
// Copyright (C) 2019 - 2020 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <iostream>
#include "db/CConnection.h"

#include "utilities/CSimConfig.h"

// static
void CConnection::init()
{
  if (pINSTANCE != NULL)
    return;

  const CSimConfig::db_connection & dbConnection = CSimConfig::getDBConnection();

  // postgresql://[user[:password]@][netloc][:port][,...][/dbname][?param1=value1&...]
  std::string URI = "postgresql://";

  if (!dbConnection.user.empty())
    {
      URI += dbConnection.user;

      if (!dbConnection.password.empty())
        URI += ":" + dbConnection.password;

      URI += "@";
    }

  URI += dbConnection.host + "/" + dbConnection.name;

  URI += "?connect_timeout=60";

  try
    {
      pINSTANCE = new CConnection(URI);
    }

  catch (const pqxx::pqxx_exception & e)
    {
      CLogger::error(CLogger::sanitize(e.base().what()));
    }
}

// static
void CConnection::release()
{
  if (pINSTANCE != NULL)
    {
      delete pINSTANCE;
      pINSTANCE = NULL;
    }
}

// static
pqxx::read_transaction * CConnection::work()
{
  if (pINSTANCE == NULL)
    return NULL;

  pqxx::read_transaction * pWork = NULL;

  try
    {
      pWork = new pqxx::read_transaction(pINSTANCE->mConnection);
    }

  catch (const std::exception & e)
    {
      CLogger::error() << "CConnection: " << CLogger::sanitize(e.what());
      pWork = NULL;
    }

  return pWork;
}

CConnection::CConnection(const std::string & uri)
  : mConnection(uri)
{}

CConnection::~CConnection()
{}
