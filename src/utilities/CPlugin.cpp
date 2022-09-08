// BEGIN: Copyright 
// Copyright (C) 2022 Rector and Visitors of the University of Virginia 
// All rights reserved 
// END: Copyright 

// BEGIN: License 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//   http://www.apache.org/licenses/LICENSE-2.0 
// END: License 

#include <stdio.h>

#include "utilities/CPlugin.h"

// static 
std::map< std::string, void * > CPlugin::Libraries;

// static
void CPlugin::Release()
{
  for (auto Library : Libraries)
    if (Library.second != NULL)
      dlclose(Library.second);

  Libraries.clear();
}

CPlugin::CPlugin(const std::string & pluginPath)
  : mPluginPath(pluginPath)
  , mpLibraryHandle(NULL)
{
  if (!mPluginPath.empty())
    {
      std::map< std::string, void * >::const_iterator found = Libraries.find(mPluginPath);

      if (found != Libraries.end())
        {
          mpLibraryHandle = found->second;
        }
      else
        {
          mpLibraryHandle = dlopen(mPluginPath.c_str(), RTLD_NOW | RTLD_DEEPBIND);

          if (mpLibraryHandle != NULL)
            {
              Libraries.insert(std::make_pair(mPluginPath, mpLibraryHandle));
            }
          else
            {
              CLogger::error() << "CPlugin" << dlerror();
            }
        }
    }
}
