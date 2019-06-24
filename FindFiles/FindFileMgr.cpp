///////////////////////////////////////////////////////////////////////
// FindFileMgr.cpp - Find names of files or dirs matching regex      //
// Ver 1.3                                                           //
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

std::string usageMsg()
{
  std::ostringstream out;
  out << "\n  FindFiles version 1.3, 24 Jun 2019";
  out << "\n  Finds files or directories with name matching a regex\n";
  out << "\n  usage: FindFiles /P path [/f] [/D] [/d] [/s] [/v] [/h] [/p pattern]* [/r regex]";
  out << "\n    path = relative or absolute path of starting directory";
  out << "\n    /f for finding files";
  out << "\n    /D for showing file dates";
  out << "\n    /d for finding directories";
  out << "\n    /s for recursive search";
  out << "\n    /v for verbose output - shows commandline processing results";
  out << "\n    /h show this message and exit";
  out << "\n    pattern is a pattern string of the form *.h,*.log, etc. with no spaces";
  out << "\n    regex is a regular expression specifying targets, e.g., files or dirs\n";
  out << "\n  Example #1: FindFiles /P ../.. /s /f /D /r \"^File|^Util\" /p *.h,*.cpp,*.cs,*.html,*.md";
  out << "\n  Example #2: FindFiles /P ../.. /s /d /r \"FindFiles$|Utilities$\" /p *.h,*.cpp,*.cs,*.html,*.md";
  out << "\n";
  return out.str();
}
void usage()
{
  std::cout << usageMsg();
}

bool FileMgr::processCmdLine(int argc, char** argv)
{
  pcl_.processCmdLine(argc, argv);
  if (pcl_.parseError())
    return false;

  if (pcl_.hasOption('h'))
  {
    usage();
    return false;
  }

  path_ = FileSystem::Path::getFullFileSpec(pcl_.path());
  if (!FileSystem::Directory::exists(path_))
  {
    std::cout << "\n  " << path_ << " does not exist\n";
    return false;
  }

  regex_ = pcl_.regex();

  if (pcl_.hasOption('s'))
  {
    recursive_ = true;
  }
  if (pcl_.hasOption('n'))
  {
    numFiles_ = pcl_.maxItems();
  }
  if (pcl_.hasOption('f') == false && pcl_.hasOption('d') == false)
  {
    pcl_.option('f');
  }
  for (Pattern patt : pcl_.patterns())
  {
    addPattern(patt);
  }
  return true;
}

FileMgr::FileMgr()
{
  pcl_.setUsageMessage(usageMsg());
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

  if (pcl_.hasOption('s'))
    find(fullPath);
  else
  {
    ++processedDirs_;
    std::cout << "\n  " << fullPath;

    std::vector<std::string> fileMatches;

    for (auto patt : pcl_.patterns())
    {
      std::vector<std::string> files = FileSystem::Directory::getFiles(fullPath, patt);
      for (auto f : files)
      {
        if (pcl_.hasOption('f'))
        {
          static std::regex re(regex_);
          if (std::regex_search(f, re))
          {
            if (pcl_.hasOption('D'))
            {
              std::string file = fullPath + "\\" + f;
              FileSystem::FileInfo fi(file);
              std::string date = fi.date();
              date = reformatDate(date);
              fileMatches.push_back(date + " -- " + f);
            }
            else
            {
              fileMatches.push_back(f);
            }
            ++processedFiles_;
          }
        }
      }
    }
    if (fileMatches.size() > 0)
    {
      for (auto file : fileMatches)
      {
        std::cout << "\n    " << file;
      }
    }
  }
}

void FileMgr::find(const Path& path)
{
  ++processedDirs_;
  if (pcl_.hasOption('d'))
  {
    static std::regex re(regex_);
    if (std::regex_search(path, re))
    {
      std::cout << "\n  " << path;
    }
  }

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
          if (pcl_.hasOption('D'))
          {
            std::string file = path + "\\" + f;
            FileSystem::FileInfo fi(file);
            std::string date = fi.date();
            date = reformatDate(date);
            fileMatches.push_back(date + " -- " + f);
          }
          else
          {
            fileMatches.push_back(f);
          }
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

using Path = std::string;
using File = std::string;

int main(int argc, char* argv[])
{
  FileMgr fm;

  if (!fm.processCmdLine(argc, argv))
  {
    return 1;
  }
  Utilities::ProcessCmdLine& pcl = fm.pcl();

  std::cout << "\n  FindFiles";

  if (fm.pcl().hasOption('v'))
  {
    fm.pcl().showCmdLine(argc, argv, true);

    std::cout << "\n    path  = " << fm.path();
    if (fm.pcl().patterns().size() > 0)
    {
      std::cout << "\n    patts = ";
      for (auto patt : fm.pcl().patterns())
      {
        std::cout << patt << " ";
      }
    }
    if (fm.pcl().options().size() > 0)
    {
      std::cout << "\n    optns = ";
      for (auto opt : fm.pcl().options())
      {
        std::cout << opt << " ";
      }
    }
    std::cout << "\n    regex = " << fm.regex() << "\n";
  }

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
