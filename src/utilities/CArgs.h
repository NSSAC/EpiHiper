// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2020 - 2024 Rector and Visitors of the University of Virginia 
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

#ifndef SRC_CARGS_H_
#define SRC_CARGS_H_

#include <string>

class CArgs
{
public:
  static bool parseArgs(int argc, char * argv[]);

  static void printUsage();

  static void printWhoAmI();

  static const std::string & getConfig();

  static const std::string & getName();

  static const std::string & getPath();

  static const std::string & getVersion();

private:
  static std::string Config;
  static std::string Name;
  static std::string Path;
  static std::string Version;
  static std::string Modified;
  
};

#endif // SRC_CARGS_H_
