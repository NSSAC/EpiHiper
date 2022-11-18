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
          std::random_device rd;
          std::this_thread::sleep_for(std::chrono::milliseconds(CRandom::uniform_int(0, dbConnection.connectionMaxDelay)(rd)));
        }

      Tries--;

      try
        {
          pINSTANCE = new CConnection(URI.str());
        }

      catch (const pqxx::pqxx_exception & e)
        {
          release();

          std::string Message = CLogger::sanitize(e.base().what());

          if ((Message.find("timeout expired") == std::string::npos
               && Message.find("Could not obtain client encoding") == std::string::npos)
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
