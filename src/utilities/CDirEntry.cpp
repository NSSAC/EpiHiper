// BEGIN: Copyright 
// MIT License 
//  
// Copyright (C) 2019 - 2023 Rector and Visitors of the University of Virginia 
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

#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef WIN32
# include <io.h>
# include <direct.h>
typedef struct _stat STAT;
# define stat _wstat
# define S_IFREG _S_IFREG
# define S_IFDIR _S_IFDIR
# define access _waccess
# define rename _wrename
# define mkdir _wmkdir
# define rmdir _wrmdir
#else
typedef struct stat STAT;
# include <dirent.h>
# include <unistd.h>
#endif // WIN32

#include "utilities/CDirEntry.h"

#include "EpiHiperConfig.h"

#ifdef WIN32
const std::string CDirEntry::Separator = "\\";
#else
const std::string CDirEntry::Separator = "/";
#endif

bool CDirEntry::isFile(const std::string & path)
{
  STAT st;

  if (stat(path.c_str(), & st) == -1) return false;

#ifdef WIN32
  return ((st.st_mode & S_IFREG) == S_IFREG);
#else
  return S_ISREG(st.st_mode);
#endif
}

bool CDirEntry::isDir(const std::string & path)
{
  STAT st;

  if (stat(path.c_str(), & st) == -1) return false;

#ifdef WIN32
  return ((st.st_mode & S_IFDIR) == S_IFDIR);
#else
  return S_ISDIR(st.st_mode);
#endif
}

bool CDirEntry::exist(const std::string & path)
{
  STAT st;

  if (stat(path.c_str(), & st) == -1) return false;

#ifdef WIN32
  return ((st.st_mode & S_IFREG) == S_IFREG ||
          (st.st_mode & S_IFDIR) == S_IFDIR);
#else
  return (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode));
#endif
}

bool CDirEntry::isReadable(const std::string & path)
{return (access(path.c_str(), 0x4) == 0);}

bool CDirEntry::isWritable(const std::string & path)
{return (access(path.c_str(), 0x2) == 0);}

std::string CDirEntry::baseName(const std::string & path)
{
  std::string::size_type start = path.find_last_of(Separator);
#ifdef WIN32 // WIN32 also understands '/' as the separator.

  if (start == std::string::npos)
    start = path.find_last_of("/");

#endif

  if (start == std::string::npos) start = 0;
  else start++; // We do not want the separator.

  std::string::size_type end = path.find_last_of(".");

  if (end == std::string::npos || end < start)
    end = path.length();

  return path.substr(start, end - start);
}

std::string CDirEntry::fileName(const std::string & path)
{
  std::string::size_type start = path.find_last_of(Separator);
#ifdef WIN32 // WIN32 also understands '/' as the separator.

  if (start == std::string::npos)
    start = path.find_last_of("/");

#endif

  if (start == std::string::npos) start = 0;
  else start++; // We do not want the separator.

  return path.substr(start);
}

std::string CDirEntry::dirName(const std::string & path)
{
  if (path == "") return path;

#ifdef WIN32 // WIN32 also understands '/' as the separator.
  std::string::size_type end = path.find_last_of(Separator + "/");
#else
  std::string::size_type end = path.find_last_of(Separator);
#endif

  if (end == path.length() - 1)
    {
#ifdef WIN32 // WIN32 also understands '/' as the separator.
      end = path.find_last_of(Separator + "/", end);
#else
      end = path.find_last_of(Separator, end);
#endif
    }

  if (end == std::string::npos) return ".";

  return path.substr(0, end);
}

std::string CDirEntry::suffix(const std::string & path)
{
  std::string::size_type start = path.find_last_of(Separator);
#ifdef WIN32 // WIN32 also understands '/' as the separator.

  if (start == std::string::npos)
    start = path.find_last_of("/");

#endif

  if (start == std::string::npos) start = 0;
  else start++; // We do not want the separator.

  std::string::size_type end = path.find_last_of(".");

  if (end == std::string::npos || end < start)
    return "";
  else
    return path.substr(end);
}

bool CDirEntry::createDir(const std::string & dir,
                          const std::string & parent)
{
  std::string Dir;

  if (!parent.empty()) Dir = parent + Separator;

  Dir += dir;

  // Check whether the directory already exists and is writable.
  if (isDir(Dir) && isWritable(Dir)) return true;

  // Check whether the parent directory exists and is writable.
  if (!parent.empty() && (!isDir(parent) || !isWritable(parent)))
    return false;

#ifdef WIN32
  return (mkdir(Dir.c_str()) == 0);
#else
  return (mkdir(Dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0);
#endif
}

bool CDirEntry::isRelativePath(const std::string & path)
{
#ifdef WIN32
  std::string Path = normalize(path);

  if (Path.length() < 2)
    return true;

  if (Path[1] == ':')
    return false;

  if (Path[0] == '/' && Path[1] == '/')
    return false;

  return true;
#else
  return (path.length() < 1 || path[0] != '/');
#endif
}

bool CDirEntry::makePathRelative(std::string & absolutePath,
                                 const std::string & relativeTo)
{
  if (isRelativePath(absolutePath) ||
      isRelativePath(relativeTo)) return false; // Nothing can be done.

  std:: string RelativeTo = normalize(relativeTo);

  if (isFile(RelativeTo)) RelativeTo = dirName(RelativeTo);

  if (!isDir(RelativeTo)) return false;

  absolutePath = normalize(absolutePath);

  size_t i, imax = std::min(absolutePath.length(), RelativeTo.length());

  for (i = 0; i < imax; i++)
    if (absolutePath[i] != RelativeTo[i]) break;

  // We need to retract to the beginning of the current directory.
  if (i != imax)
    i = absolutePath.find_last_of('/', i) + 1;

#ifdef WIN32

  if (i == 0) return false; // A different drive letter we cannot do anything

#endif

  RelativeTo = RelativeTo.substr(i);

  std::string relativePath;

  while (RelativeTo != "")
    {
      relativePath += "../";
      RelativeTo = dirName(RelativeTo);
    }

  if (relativePath != "")
    absolutePath = relativePath + absolutePath.substr(i);
  else
    absolutePath = absolutePath.substr(i + 1);

  return true;
}

bool CDirEntry::makePathAbsolute(std::string & relativePath,
                                 const std::string & absoluteTo)
{
  if (!isRelativePath(relativePath) ||
      isRelativePath(absoluteTo)) return false; // Nothing can be done.

  std:: string AbsoluteTo = normalize(absoluteTo);

  if (isFile(AbsoluteTo)) AbsoluteTo = dirName(AbsoluteTo);

  if (!isDir(AbsoluteTo)) return false;

  relativePath = normalize(relativePath);

  while (!relativePath.compare(0, 3, "../"))
    {
      AbsoluteTo = dirName(AbsoluteTo);
      relativePath = relativePath.substr(3);
    }

  relativePath = AbsoluteTo + "/" + relativePath;

  return true;
}

std::string CDirEntry::normalize(const std::string & path)
{
  if (path.empty())
    return "";

  std::string Normalized = path;

#ifdef WIN32
  // converts all '\' to '/' (only on WIN32)
  size_t i, imax;

  for (i = 0, imax = Normalized.length(); i < imax; i++)
    if (Normalized[i] == '\\') Normalized[i] = '/';

  // if the path starts with /[^/] we prepend the current drive letter
  if (Normalized.length() > 0 && Normalized[0] == '/')
    {
      if ((Normalized.length() > 1 && Normalized[1] != '/') ||
          Normalized.length() == 1)
        {
          std::string PWD;
          COptions::getValue("PWD", PWD);

          Normalized = PWD.substr(0, 2) + Normalized;
        }
    }

#endif

  // Remove leading './'
  while (!Normalized.compare(0, 2, "./"))
    Normalized = Normalized.substr(2);

  // Remove trailing '/'
  while (Normalized[Normalized.length() - 1] == '/')
    Normalized.erase(Normalized.length() - 1);

  // Collapse '//' to '/'
  std::string::size_type pos = 1;

  while (true)
    {
      pos = Normalized.find("//", pos);

      if (pos == std::string::npos) break;

      Normalized.erase(pos, 1);
    }

  // Collapse '/./' to '/'
  pos = 0;

  while (true)
    {
      pos = Normalized.find("/./", pos);

      if (pos == std::string::npos) break;

      Normalized.erase(pos, 2);
    }

  // Collapse '[^/]+/../' to '/'
  std::string::size_type start = Normalized.length();

  while (true)
    {
      pos = Normalized.rfind("/../", start);

      if (pos == std::string::npos) break;

      start = Normalized.rfind('/', pos - 1);

      if (start == std::string::npos) break;

      if (!Normalized.compare(start, 4, "/../")) continue;

      Normalized.erase(start, pos - start + 3);
      start = Normalized.length();
    }

  return Normalized;
}

std::string CDirEntry::getPWD(void)
{
#ifdef WIN32
  int PWDSize = 256;
  typedef wchar_t lchar;
#else // WIN32
  size_t PWDSize = 256;
  typedef char lchar;
#endif // not WIN32

  lchar * PWD = NULL;

  while (!(PWD = getcwd(NULL, PWDSize)))
    {
      if (errno != ERANGE) break;

      PWDSize *= 2;
    }

  std::string pwd;

  if (PWD)
    {
      pwd = PWD;
      free(PWD);
    }

  return pwd;
}

// static
std::string CDirEntry::resolve(const std::string & pathSpec,
                               const std::string & relativeTo,
                               const std::string & defaultTo)
{
  if (pathSpec.substr(0, 7) == "self://")
    {
      std::string Path(pathSpec.substr(7));
      makePathAbsolute(Path, relativeTo);

      return Path;
    }

  if (isRelativePath(pathSpec))
    {
      std::string Path(pathSpec);

      if (defaultTo.empty())
        makePathAbsolute(Path, getPWD());
      else
        makePathAbsolute(Path, defaultTo);

      return Path;
    }

  return pathSpec;
}

bool CDirEntry::remove(const std::string & path)
{
  if (isDir(path))
    return (rmdir(path.c_str()) == 0);
  else if (isFile(path))
#ifdef WIN32
    return (_wremove(CLocaleString::fromUtf8(path).c_str()) == 0);

#else
    return (::remove(path.c_str()) == 0);
#endif

  return false;
}

