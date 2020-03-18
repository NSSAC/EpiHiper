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
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include "db/CConnection.h"

#include "utilities/CSimConfig.h"
#include "utilities/CRandom.h"

// static
void CConnection::setRequired(const bool & isRequired)
{
  required = isRequired;
}

// static
void CConnection::init()
{
  if (pINSTANCE != NULL
      || !required)
    return;

  const CSimConfig::db_connection & dbConnection = CSimConfig::getDBConnection();

  // postgresql://[user[:password]@][netloc][:port][,...][/dbname][?param1=value1&...]
  std::ostringstream URI;
  URI << "postgresql://";

  if (!dbConnection.user.empty())
    {
      URI << dbConnection.user;

      if (!dbConnection.password.empty())
        URI << ":" << dbConnection.password;

      URI << "@";
    }

  URI << dbConnection.host <<"/" << dbConnection.name;

  URI << "?connect_timeout=" << dbConnection.connectionTimeout;
  int Tries = dbConnection.connectionRetries + 1;

  while (Tries > 0
         && pINSTANCE == NULL)
    {
      if (dbConnection.connectionMaxDelay > 0)
        {
          std::this_thread::sleep_for(std::chrono::milliseconds(CRandom::uniform_int(0, dbConnection.connectionMaxDelay)(CRandom::G)));
        }

      Tries--;

      try
        {
          pINSTANCE = new CConnection(URI.str());
        }

      catch (const pqxx::pqxx_exception & e)
        {
          std::string Message = CLogger::sanitize(e.base().what());

          if (Message.find("timeout expired") == std::string::npos
              || Tries == 0)
            {
              CLogger::error() << Message;
              Tries = 0;
            } 
          else
            {
              CLogger::warn() << Message << " tries left: " << Tries;
            }
        }
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
