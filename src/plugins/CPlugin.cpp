// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2022 - 2023 Rector and Visitors of the University of Virginia 
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
              CLogger::warn("CPlugin {}: Plugin already loaded.", PluginPath);
              continue;
            }
          else
            {
#ifdef TARGET_MACOSX
              pLibraryHandle = dlopen(PluginPath.c_str(), RTLD_NOW);
#else
              pLibraryHandle = dlopen(PluginPath.c_str(), RTLD_NOW | RTLD_DEEPBIND);
#endif // TARGET_MACOSX
              if (pLibraryHandle != nullptr)
                {
                  Libraries.emplace(PluginPath, pLibraryHandle);
                }
              else
                {
                  CLogger::error("CPlugin {}", dlerror());
                  continue;
                }
            }

          char * error;
          EpiHiperPluginInit pInit = (EpiHiperPluginInit) dlsym(pLibraryHandle, "EpiHiperPluginInit");

          if ((error = dlerror()) != nullptr)
            {
              CLogger::error("CPlugin {}", error);
              continue;
            }

          CLogger::info("CPlugin {}: Calling EpiHiperPluginInit().", PluginPath);
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
