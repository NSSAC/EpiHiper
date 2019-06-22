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

#include "DirEntry.h"

#ifdef WIN32
const std::string DirEntry::Separator = "\\";
#else
const std::string DirEntry::Separator = "/";
#endif

bool DirEntry::isFile(const std::string & path)
{
  STAT st;

  if (stat(path.c_str(), & st) == -1) return false;

#ifdef WIN32
  return ((st.st_mode & S_IFREG) == S_IFREG);
#else
  return S_ISREG(st.st_mode);
#endif
}

bool DirEntry::isDir(const std::string & path)
{
  STAT st;

  if (stat(path.c_str(), & st) == -1) return false;

#ifdef WIN32
  return ((st.st_mode & S_IFDIR) == S_IFDIR);
#else
  return S_ISDIR(st.st_mode);
#endif
}

bool DirEntry::exist(const std::string & path)
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

bool DirEntry::isReadable(const std::string & path)
{return (access(path.c_str(), 0x4) == 0);}

bool DirEntry::isWritable(const std::string & path)
{return (access(path.c_str(), 0x2) == 0);}

std::string DirEntry::baseName(const std::string & path)
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

std::string DirEntry::fileName(const std::string & path)
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

std::string DirEntry::dirName(const std::string & path)
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

  if (end == std::string::npos) return "";

  return path.substr(0, end);
}

std::string DirEntry::suffix(const std::string & path)
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

bool DirEntry::createDir(const std::string & dir,
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

bool DirEntry::isRelativePath(const std::string & path)
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

bool DirEntry::makePathRelative(std::string & absolutePath,
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

bool DirEntry::makePathAbsolute(std::string & relativePath,
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

std::string DirEntry::normalize(const std::string & path)
{
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

std::string DirEntry::getPWD(void)
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

