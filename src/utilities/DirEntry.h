#ifndef SRC_UTILITIES_DIRENTRY_H_
#define SRC_UTILITIES_DIRENTRY_H_

#include <string>
#include <vector>

/**
 * This class provides an OS independent interface to directory entries
 * such as files and directories.
 */
class DirEntry
{
public:
  /**
   * The character used to separate directory entries.
   */
  static const std::string Separator;

  /**
   * Check whether the directory entry specified by 'path' is
   * a file.
   * @param const std::string & path
   * @return bool isFile
   */
  static bool isFile(const std::string & path);

  /**
   * Check whether the directory entry specified by 'path' is
   * is a directory.
   * @param const std::string & path
   * @return bool isDir
   */
  static bool isDir(const std::string & path);

  /**
   * Check whether the directory entry specified by 'path' exists.
   * @param const std::string & path
   * @return bool exist
   */
  static bool exist(const std::string & path);

  /**
   * Check whether the directory entry specified by 'path' is
   * is readable.
   * @param const std::string & path
   * @return bool isReadable
   */
  static bool isReadable(const std::string & path);

  /**
   * Check whether the directory entry specified by 'path' is
   * writable.
   * @param const std::string & path
   * @return bool isWritable
   */
  static bool isWritable(const std::string & path);

  /**
   * Returns the base name, i.e., the directory path and the
   * the suffix are removed from 'path'.
   * @param const std::string & path
   * @return std::string baseName
   */
  static std::string baseName(const std::string & path);

  /**
   * Returns the file name, i.e., the directory path is removed from 'path'.
   * @param const std::string & path
   * @return std::string fileName
   */
  static std::string fileName(const std::string & path);

  /**
   * Returns the directory path to the parent directoryu, i.e.,
   * the file name or directory name are removed from 'path'.
   * @param const std::string & path
   * @return std::string dirName
   */
  static std::string dirName(const std::string & path);

  /**
   * Returns the suffix, i.e., the directory path and the
   * the base name are removed from 'path'.
   * @param const std::string & path
   * @return std::string basename
   */
  static std::string suffix(const std::string & path);

  /**
   * Create the directory 'dir' in the parent directory 'parent'.
   * @param const std::string & dir
   * @param const std::string & parent (Default: current working directory)
   * @return bool success
   */
  static bool createDir(const std::string & dir,
                        const std::string & parent = "");

  /**
   * Checks whether the given path is relative
   * @return bool isRelative
   */
  static bool isRelativePath(const std::string & path);

  /**
   * Makes the absolute path relative to the path given in relativeTo
   * @param std::string & absolutePath
   * @param const std::string & relativeTo
   * @return bool success
   */
  static bool makePathRelative(std::string & absolutePath,
                               const std::string & relativeTo);

  /**
   * Makes the relative path absolute to the path given in absoluteTo
   * @param std::string & relativePath
   * @param const std::string & absoluteTo
   * @return bool success
   */
  static bool makePathAbsolute(std::string & relativePath,
                               const std::string & absoluteTo);

  /**
   * This method normalizes the path, i.e.,
   * it converts all '\' to '/' (only on WIN32)
   * and collapses '^./' to '^', '/./' to '/', and '[^/]+/../' to '/'
   * @param const std::string & path
   * @return std::string normalizedPath
   */
  static std::string normalize(const std::string & path);

  /**
   * Retrieve the current working directory
   * @return std::string pwd
   */
  static std::string getPWD(void);

};

#endif // SRC_UTILITIES_DIRENTRY_H_
