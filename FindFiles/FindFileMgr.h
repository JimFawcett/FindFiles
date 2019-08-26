#ifndef FINDFILEMGR_H
#define FINDFILEMGR_H
///////////////////////////////////////////////////////////////////////
// FindFileMgr.h - Find dates of files matching specified patterns   //
// Ver 1.3                                                           //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018           //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * FindFileMgr uses the services of FileSystem to find files.
 * - Finds all files, matching a regular expression, along with their paths.
 * - Filters files by pattern before searching for regex match.
 *
 * Required Files:
 * ---------------
 * FindFileMgr.h, FindFileMgr.cpp
 * FileSystem.h, FileSystem.cpp,
 * CodeUtilities.h, 
 * StringUtilities.h
 *
 * Maintenance History:
 * --------------------
 * Ver 1.3 : 24 Jun 2019
 * - fixed bug in non-recursive operation
 * - fixed bugs in options processing
 * Ver 1.2 : 24 Jun 2019
 * - added option to show file dates
 * Ver 1.1 : 24 Jun 2019
 * - added /d option for finding directories
 * Ver 1.0 : 22 Jun 2019
 * - first release
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
//#include "../Utilities/CodeUtilities/CodeUtilities.h"
#include "../CppUtilities/CodeUtilities/CodeUtilities.h"

class FileMgr
{
public:
  using Path = std::string;
  using Pattern = std::string;
  using Patterns = std::vector<Pattern>;
  using File = std::string;
  using Regex = std::string;
  using Date = std::string;
  using DataStore = std::multimap<Date, File, std::greater<Date>>;
  using DataItem = DataStore::value_type;
  using const_iterator = DataStore::const_iterator;
  using const_reverse_iterator = DataStore::const_reverse_iterator;

  FileMgr();
  bool processCmdLine(int argc, char** argv);
  Utilities::ProcessCmdLine& pcl();
  void path(const Path& path);
  Path path();
  void regex(const Regex& rx);
  Regex regex();
  void recursive(bool recurse);
  bool recursive();
  void numFiles(size_t num);
  size_t numFiles();
  void addPattern(const std::string& patt);
  void search();
  void find(const Path& path);
  void showProcessed();
private:
  Date reformatDate(const Date& date);
  Utilities::ProcessCmdLine pcl_;
  Path path_;
  Patterns patterns_;
  Regex regex_ = ".*";
  bool recursive_ = false;
  size_t numFiles_ = 0;
  size_t processedFiles_ = 0;
  size_t processedDirs_ = 0;
};

inline Utilities::ProcessCmdLine& FileMgr::pcl()
{
  return pcl_;
}
inline void FileMgr::path(const Path& path)
{
  path_ = path;
}

inline FileMgr::Path FileMgr::path()
{
  return path_;
}

inline FileMgr::Regex FileMgr::regex()
{
  return regex_;
}

inline void FileMgr::regex(const Regex& rx)
{
  regex_ = rx;
}

inline void FileMgr::numFiles(size_t num) 
{ 
  numFiles_ = num; 
}

inline size_t FileMgr::numFiles() 
{ 
  return numFiles_; 
}

inline void FileMgr::recursive(bool recurse)
{
  recursive_ = recurse;
}

inline bool FileMgr::recursive()
{
  return recursive_;
}

#endif
