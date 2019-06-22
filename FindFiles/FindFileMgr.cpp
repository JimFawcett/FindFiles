///////////////////////////////////////////////////////////////////////
// FindFileMgr.cpp - Find names of files or dirs matching regex      //
// Ver 1.0                                                           //
// Jim Fawcett, https://github.com/JimFawcett/FindFiles, Summer 2019 //
///////////////////////////////////////////////////////////////////////

#include "FindFileMgr.h"
#include "FileSystem.h"
#include "../Utilities/CodeUtilities/CodeUtilities.h"
//#define STATIC_LIB
#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <algorithm>
#include <regex>

void usage()
{
  std::cout << "\n  FindFiles version 1.0, 22 Jun 2019";
  std::cout << "\n  Finds files or directories with name matching a regex\n";
  std::cout << "\n  usage: FileDates /P path [/f] [/d] [/s] [/p pattern]* [/n numFiles]";
  std::cout << "\n    path = relative or absolute path of starting directory";
  std::cout << "\n    /s for recursive search";
  std::cout << "\n    pattern is a pattern string of the form *.h,*.log, etc. with no spaces";
  std::cout << "\n    numFiles is the number of files displayed";
  std::cout << "\n  Example: FindFiles ../.. /s /d /r File|Dir";
  std::cout << "\n";
}
bool FileMgr::processCmdLine(int argc, char** argv)
{
  if (argc < 2)
  {
    usage();
    return false;
  }

  pcl_.processCmdLine(argc, argv);
  if (pcl_.parseError())
    return false;

  path_ = FileSystem::Path::getFullFileSpec(pcl_.path());
  if (!FileSystem::Directory::exists(path_))
  {
    std::cout << "\n  " << path_ << " does not exist\n";
    return false;
  }

  for (int i = 2; i < argc; ++i)
  {
    regex_ = pcl_.regex();

    if (pcl_.hasOption('s'))
    {
      recursive_ = true;
    }
    if (pcl_.hasOption('n'))
    {
      numFiles_ = pcl_.maxItems();
    }
  }
  for (Pattern patt : pcl_.patterns())
  {
    addPattern(patt);
  }
  return true;
}

FileMgr::FileMgr()
{
  //patterns_.push_back("*.*");
}

void FileMgr::addPattern(const std::string& patt)
{
  if (patterns_.size() == 1 && patterns_[0] == "*.*")
    patterns_.pop_back();
  patterns_.push_back(patt);
}

FileMgr::Date FileMgr::reformatDate(const Date& datetime)
{
  size_t pos = datetime.find(' ');
  if (pos >= datetime.size())
    throw "bad date";
  std::string date = datetime.substr(0, pos);    // month/day/year
  std::string time = datetime.substr(pos + 1);   // hr:min:sec

  pos = date.find_last_of('/');
  if (pos >= date.size())
    throw "bad date";
  std::string temp = date.substr(0, pos);    // month/day
  std::string year = date.substr(pos + 1);   // year

  pos = temp.find_last_of('/');
  if (pos >= date.size())
    throw "bad date";
  std::string month = temp.substr(0, pos);   // month
  std::string day = temp.substr(pos + 1);    // day

  std::string reformattedDate = year + "/" + month + "/" + day;
  std::string reformattedDateTime = reformattedDate + " " + time;
  return reformattedDateTime;
}

void FileMgr::search()
{
  std::string fullPath = FileSystem::Path::getFullFileSpec(path_);
  if (recursive_)
    find(fullPath);
  else
  {
    std::vector<std::string> fileMatches;

    for (auto patt : pcl_.patterns())
    {
      std::vector<std::string> files = FileSystem::Directory::getFiles(fullPath, patt);
      for (auto file : files)
      {
        if (pcl_.hasOption('f'))
        {
          static std::regex re(regex_);
          if (std::regex_search(file, re))
          {
            fileMatches.push_back(file);
            ++processedFiles_;
          }
        }
      }
    }
  }
}

void FileMgr::find(const Path& path)
{
  std::vector<std::string> fileMatches;

  for (auto patt : pcl_.patterns())
  {
    std::vector<std::string> files = FileSystem::Directory::getFiles(path, patt);

    for (auto f : files)
    {
      if (pcl_.hasOption('f'))
      {
        static std::regex re(regex_);
        if (std::regex_search(f, re))
        {
          fileMatches.push_back(f);
          ++processedFiles_;
        }
      }
    }
  }
  if (fileMatches.size() > 0)
  {
    std::cout << "\n  " << path;
    for (auto file : fileMatches)
    {
      std::cout << "\n    " << file;
    }
  }
  std::vector<std::string> subdirs = FileSystem::Directory::getDirectories(path);
  for (auto d : subdirs)
  {
    if (d != "." && d != "..")
    {
      find(path + "\\" + d);
      ++processedDirs_;
    }
  }
}

void FileMgr::showProcessed()
{
  std::cout << "\n\n    Processed " << processedFiles_ << " files";
  std::cout << "\n    Processed " << processedDirs_ << " dirs";
}

#ifdef TEST_FINDFILEMGR

#include <map>
#include <iostream>
#include <functional>
#include "../Utilities/CodeUtilities/CodeUtilities.h"
#include "../Utilities/StringUtilities/StringUtilities.h"

//using Date = std::string;
using Path = std::string;
using File = std::string;

int main(int argc, char* argv[])
{
  FileMgr fm;
  fm.processCmdLine(argc, argv);

  Utilities::ProcessCmdLine& pcl = fm.pcl();

  std::cout << "\n  FindFiles";
  fm.pcl().showCmdLine(argc, argv);
  std::cout << "\n    path  = " << fm.path();
  if (fm.pcl().patterns().size() > 0)
  {
    std::cout << "\n    patts = ";
    for (auto patt : fm.pcl().patterns())
    {
      std::cout << patt << " ";
    }
  }
  std::cout << "\n    regex = " << fm.regex() << "\n";

  fm.path(fm.pcl().path());
  for (auto patt : fm.pcl().patterns())
    fm.addPattern(patt);
  
  if (pcl.hasOption('s'))
    fm.recursive(true);

  if(pcl.parseError())
  {
    std::cout << "\n    command line parsing failed\n\n";
    return 1;
  }

  fm.search();
  fm.showProcessed();

  std::cout << "\n\n";
  return 0;
}

#endif