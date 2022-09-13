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

#include "plugins/CPlugin.h"
#include "utilities/CSimConfig.h"

// static
std::map< std::string, void * > CPlugin::Libraries;

// static
void CPlugin::Init()
{
  for (const std::string & PluginPath : CSimConfig::getPlugins())
    {
      void * pLibraryHandle = nullptr;

      if (!PluginPath.empty())
        {
          std::map< std::string, void * >::const_iterator found = Libraries.find(PluginPath);

          if (found != Libraries.end())
            {
              CLogger::warn() << "CPlugin " << PluginPath << ": Plugin already loaded.";
              continue;
            }
          else
            {
              pLibraryHandle = dlopen(PluginPath.c_str(), RTLD_NOW | RTLD_DEEPBIND);

              if (pLibraryHandle != nullptr)
                {
                  Libraries.insert(std::make_pair(PluginPath, pLibraryHandle));
                }
              else
                {
                  CLogger::error() << "CPlugin" << dlerror();
                  continue;
                }
            }

          char * error;
          EpiHiperPluginInit pInit = (EpiHiperPluginInit) dlsym(pLibraryHandle, "EpiHiperPluginInit");

          if ((error = dlerror()) != nullptr)
            {
              CLogger::error() << "CPlugin " << error;
              continue;
            }

          CLogger::info() << "CPlugin " << PluginPath << ": Calling EpiHiperPluginInit().";
          (*pInit)();
        }
    }
}

// static
void CPlugin::Release()
{
  for (auto Library : Libraries)
    if (Library.second != nullptr)
      dlclose(Library.second);

  Libraries.clear();
}
