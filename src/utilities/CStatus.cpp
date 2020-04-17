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

#include <cstring>
#include <jansson.h>

#include "utilities/CStatus.h"
#include "utilities/CSimConfig.h"
#include "utilities/CArgs.h"
#include "utilities/CDirEntry.h"
#include "actions/CActionQueue.h"

// static
std::string CStatus::fileName;

json_t * init(json_t * pObject, const char * key, const char * value)
{
  json_t * pName = json_object_get(pObject, key);

  if (!json_is_string(pName))
    {
      json_object_del(pObject, key);
      json_object_set_new(pObject, key, json_string(value));
      pName = json_object_get(pObject, key);
    }

  return pName;
}

json_t * init(json_t * pObject, const char * key, const double & value)
{
  json_t * pName = json_object_get(pObject, key);

  if (!json_is_real(pName))
    {
      json_object_del(pObject, key);
      json_object_set_new(pObject, key, json_real(value));
      pName = json_object_get(pObject, key);
    }

  return pName;
}


// static
void CStatus::load(const std::string & id, const std::string & name, const std::string &file)
{
  fileName = file;

  if (CCommunicate::MPIRank != 0) return;

  if (CDirEntry::exist(fileName))
    {
      pJSON = CSimConfig::loadJson(fileName, JSON_DECODE_INT_AS_REAL);
    }

  if (pJSON == NULL)
    {
      std::string Default("{\"id\":\"" + id + "\",\"name\":\"" + name + "\",\"status\":\"running\",\"progress\":0.0,\"detail\":\"Starting\"}");

      json_error_t error;

      pJSON = json_loads(Default.c_str(), JSON_DECODE_INT_AS_REAL, &error);
    }

  pId = init(pJSON, "id", id.c_str());
  pName = init(pJSON, "name", name.c_str());
  pStatus = init(pJSON, "status", "running");
  pDetail = init(pJSON, "detail", (name + ": Starting").c_str());
  pProgress = init(pJSON, "progress", 0.0);

  json_real_set(pProgress, 0.0);

  if (strcmp(json_string_value(pId), id.c_str()) == 0 &&
      CDirEntry::exist(CDirEntry::dirName(fileName) + "/job.json"))
    {
      json_t * pRoot = CSimConfig::loadJson(CDirEntry::dirName(fileName) + "/job.json", JSON_DECODE_INT_AS_REAL);
      json_t * pValue = json_object_get(pRoot, "id");

      if (json_is_string(pValue))
        {
          json_string_set(pId, json_string_value(pValue));
        }

      json_decref(pRoot);
    }

  update("running");
}

// static
void CStatus::update(const std::string & status, const double & progress)
{
  if (CCommunicate::MPIRank != 0) return;

  json_string_set(pStatus, status.c_str());

  if (!std::isnan(progress))
    json_real_set(pProgress, progress);

  std::string Name = json_string_value(pName);

  if (status == "running")
    {
      if (progress == 0.0)
        json_string_set(pDetail,  (Name + ": Starting").c_str());
      else
        json_string_set(pDetail,  (Name + ": Running").c_str());
    }
  else if (status == "completed")
    {
      json_string_set(pDetail,  (Name + ": Completed").c_str());
    }
  else // status must be "failed"
    {
      json_string_set(pDetail,  (Name + ": Failed").c_str());
    }

  json_dump_file(pJSON, fileName.c_str(), JSON_INDENT(2));
}

// static
void CStatus::success()
{
  update("completed", 100.0);
  json_decref(pJSON);
}

