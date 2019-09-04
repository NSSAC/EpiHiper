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

#include <cstring>
#include <jansson.h>

#include "utilities/CStatus.h"
#include "utilities/CSimConfig.h"
#include "utilities/CDirEntry.h"
#include "actions/CActionQueue.h"

// static
json_t * CStatus::pJSON(NULL);

// static
json_t * CStatus::pId(NULL);

// static
json_t * CStatus::pName(NULL);

// static
json_t * CStatus::pStatus(NULL);

// static
json_t * CStatus::pDetail(NULL);

// static
json_t * CStatus::pProgress(NULL);

// static
double CStatus::initialProgress(0.0);

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
void CStatus::load()
{
  if (CCommunicate::MPIRank != 0) return;

  if (CDirEntry::exist(CSimConfig::getStatus()))
    {
      pJSON = CSimConfig::loadJson(CSimConfig::getStatus(), JSON_DECODE_INT_AS_REAL);
    }

  if (pJSON == NULL)
    {
      std::string Default("{\"id\":\"epihiper\",\"name\":\"EpiHiper\",\"status\":\"running\",\"progress\":0.0,\"detail\":\"Starting\"}");

      json_error_t error;

      pJSON = json_loads(Default.c_str(), JSON_DECODE_INT_AS_REAL, &error);
    }

  pId = init(pJSON, "id", "epihiper");
  pName = init(pJSON, "name", "EpiHiper");
  pStatus = init(pJSON, "status", "running");
  pDetail = init(pJSON, "detail", "EpiHiper: Starting");
  pProgress = init(pJSON, "progress", 0.0);

  initialProgress = json_real_value(pProgress);

  if (strcmp(json_string_value(pId), "epihiper") == 0 &&
      CDirEntry::exist(CDirEntry::dirName(CSimConfig::getStatus()) + "/job.json"))
    {
      json_t * pRoot = CSimConfig::loadJson(CDirEntry::dirName(CSimConfig::getStatus()) + "/job.json", JSON_DECODE_INT_AS_REAL);
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
void CStatus::update(const std::string & status)
{
  if (CCommunicate::MPIRank != 0) return;

  json_string_set(pStatus, status.c_str());

  double Progress = initialProgress + ((100.0 - initialProgress) * std::max((CActionQueue::getCurrentTick() - CSimConfig::getStartTick() + 1), 0)) / (CSimConfig::getEndTick() - CSimConfig::getStartTick() + 1);
  json_real_set(pProgress, Progress);

  if (status == "running")
    {
      if (Progress == initialProgress)
        json_string_set(pDetail, "EpiHiper: Starting");
      else
        json_string_set(pDetail, "EpiHiper: Running");
    }
  else if (status == "completed")
    {
      json_string_set(pDetail, "EpiHiper: Completed");
    }
  else // status must be "failed"
    {
      json_string_set(pDetail, "EpiHiper: Failed");
    }

  json_dump_file(pJSON, CSimConfig::getStatus().c_str(), JSON_INDENT(2));
}

// static
void CStatus::finalize()
{
  update("completed");
  json_decref(pJSON);
}

