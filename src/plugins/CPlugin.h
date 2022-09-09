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

#ifndef SRC_UTILITIES_CPLUGIN_H_
#define SRC_UTILITIES_CPLUGIN_H_

#include <map>
#include <string>
#include <dlfcn.h>

#include "utilities/CLogger.h"

class CPlugin
{
private:
  static std::map< std::string, void * > Libraries;

public:
  static void Init();
  static void Release();

  CPlugin(const std::string & pluginPath);

  template < class CType > CType symbol(const std::string & symbol) const;

private:
  std::string mPluginPath;
  void * mpLibraryHandle;
};

template < class CType >
CType CPlugin::symbol(const std::string & symbol) const
{
  char * error;
  CType pSymbol = nullptr;

  if (mpLibraryHandle != nullptr)
    {
      pSymbol = (CType) dlsym(mpLibraryHandle, symbol.c_str());

      if ((error = dlerror()) != nullptr)
        {
          CLogger::error() << "CPlugin " << error;
          pSymbol = nullptr;
        }
    }

  return pSymbol;
}

#endif // SRC_UTILITIES_CPLUGIN_H_