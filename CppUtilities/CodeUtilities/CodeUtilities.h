#pragma once
/////////////////////////////////////////////////////////////////////
// CodeUtilities.h - small, generally useful, helper classes       //
// ver 1.7                                                         //
//-----------------------------------------------------------------//
// Jim Fawcett (c) copyright 2019                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Jim Fawcett, Emeritus Teaching Professor, Syracuse University   //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides classes:
* - ProcessCmdLine    extracts path, options, patterns, and a number from command line
* - Converter<T>      converts T to and from strings
* - Box<T>            converts primitive type to instance of a class
* - PersistFactory<T> adds toXml() method to T
*
* Build Process:
* --------------
* Required Files: 
*   CodeUtilities.h
*
* Maintenance History:
* --------------------
* ver 1.7 : 04 Aug 2019
* - replaced local option storage with pcl object
* ver 1.6 : 01 Aug 2019
* - converted options storage to std::unordered_map
* ver 1.5 : 02 Jul 2019
* - added capturing name of log file from command line option /F logfile.txt
* ver 1.4 : 25 Jun 2019
* - moved processing from constructor to function process
* ver 1.3 : 24 Jun 2019
* - modified options processing - now have:
*     default path = "."
*     default pattern = "*.*"
* - fixed test stub output
* ver 1.2 : 22 Jun 2019
* - redesigned handling of commandline args in ProcessCmdLine
* - now expects path to be preceded by /P
* - now expects pattern to be preceeded by /p and accepts *.h,*.cpp,.. with no spaces
* - now expects number to be preceeded by /n
* ver 1.1 : 10 Aug 2018
* - added ProcessCmdLine class
* ver 1.0 : 12 Jan 2018
* - first release
* - refactored from earlier Utilities.h
*
* Notes:
* ------
* - Designed to provide all functionality in header file. 
* - Implementation file only needed for test and demo.
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/

#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include "..//StringUtilities/StringUtilities.h"

namespace Utilities
{
  /////////////////////////////////////////////////////////////////////
  // ProcessCmdLine class
  // - extracts path, patterns, options, number

  class ProcessCmdLine
  {
  public:
    using Path = std::string;
    using Option = char;
    using OptionValue = std::string;
    using Options = std::unordered_map<Option, OptionValue>;
    using iterator = std::unordered_map<Option, OptionValue>::iterator;
    using Pattern = std::string;
    using Patterns = std::vector<Pattern>;
    using Regex = std::string;
    using LogFile = std::string;
    using Number = long int;

    ProcessCmdLine(int argc, char** argv, std::ostream& out = std::cout);
    ProcessCmdLine() : pOut_(&std::cout) {};
    ProcessCmdLine(const ProcessCmdLine& pcl) = delete;
    ProcessCmdLine& operator=(const ProcessCmdLine& pcl) = delete;
    void process();
    void process(int argc, char** argv);
    bool parseError();
    Path path();
    void path(const Path& path);
    Options& options();
    void option(Option op, OptionValue val = "");
    bool hasOption(Option op);
    Patterns patterns();
    void pattern(const Pattern& patt);
    Number maxItems();
    void maxItems(Number number);
    Regex regex();
    void regex(const Regex& rx);
    LogFile logFile();
    void logFile(const LogFile& lf);
    void usage(const std::string& msg = "");
    void showCmdLine(int argc, char** argv, bool showFirst = false);
    void showParse();
    void showPath();
    void showOptions();
    void showPatterns();
    void showMaxItems();
    void showRegex();
    void showLogFile();
    void setUsageMessage(const std::string& msg);
  private:
    void defaultUsageMessage();
    int argc_ = 0;
    std::vector<char*> argv_;
    Patterns patterns_ = Patterns();
    Options options_ = Options();
    bool parseError_ = false;
    std::ostream* pOut_;
    std::ostringstream msg_ = std::ostringstream();
  };

  /////////////////////////////////////////////////////////////////////
  // preface function
  // - used to add a string preface to an output, e.g., "\n  "

  inline void preface(const std::string& msg = "", bool doReturn = true, std::ostream& out = std::cout, const std::string& prefix = "  ")
  {
    if (doReturn) out << "\n" << prefix << msg;
  }

  /*----< path operations >------------------------------------------*/

  inline void ProcessCmdLine::path(const Path& path) 
  { 
    options_['P'] = path;
  }

  inline ProcessCmdLine::Path ProcessCmdLine::path()
  {
    iterator iter = options_.find('P');
    if (iter == options_.end())
      return ".";
    return iter->second;
  }

  inline void ProcessCmdLine::showPath()
  {
    *pOut_ << path() << " ";
  }

  /*----< options operations >---------------------------------------*/

  inline void ProcessCmdLine::option(Option option, OptionValue optionValue)
  {
    options_[option] = optionValue;
  }

  inline ProcessCmdLine::Options& ProcessCmdLine::options()
  { 
    return options_; 
  }

  inline bool ProcessCmdLine::hasOption(Option opt)
  {
    return options_.count(opt) > 0;
  }

  inline void ProcessCmdLine::showOptions()
  {
    for (auto opt : options_)
    {
      *pOut_ << '/' << opt.first << " " << opt.second << " ";
    }
  }

  /*----< patterns operations >--------------------------------------*/

  inline void ProcessCmdLine::pattern(const Pattern& pattern)
  {
    if (patterns_.size() == 1 && patterns_[0] == "*.*")
      patterns_.erase(patterns_.begin());
    patterns_.push_back(pattern);
  }

  inline ProcessCmdLine::Patterns ProcessCmdLine::patterns()
  {
    if (patterns_.size() == 0)
      patterns_.push_back("*.*");
    return patterns_;
  }

  inline void ProcessCmdLine::showPatterns()
  {
    for (auto patt : patterns_)
    {
      *pOut_ << patt << " ";
    }
  }

  /*----< maxItems operations >--------------------------------------*/

  inline void ProcessCmdLine::maxItems(Number maxItems)
  {
    options_['n'] = std::to_string(maxItems);
  }

  inline ProcessCmdLine::Number ProcessCmdLine::maxItems()
  {
    iterator iter = options_.find('n');
    if (iter == options_.end())
      return 0;
    return std::stol(iter->second);
  }

  inline void ProcessCmdLine::showMaxItems()
  {
    *pOut_ << maxItems() << " ";
  }

  /*----< regex operations >-----------------------------------------*/

  inline void ProcessCmdLine::regex(const Regex& rx)
  {
    options_['r'] = rx;
  }

  inline ProcessCmdLine::Regex ProcessCmdLine::regex()
  {
    iterator iter = options_.find('R');
    if (iter == options_.end())
      return ".*";
    if (iter->second == "")
      parseError_ = true;
    return iter->second;
  }

  inline void ProcessCmdLine::showRegex()
  {
    *pOut_ << regex() << " ";
  }

  /*----< LogFile operations >---------------------------------------*/

  inline void ProcessCmdLine::logFile(const LogFile& lf)
  {
    options_['F'] = lf;
  }

  inline ProcessCmdLine::LogFile ProcessCmdLine::logFile()
  {
    iterator iter = options_.find('F');
    if (iter == options_.end())
      return "";
    if (iter->second == "")
      parseError_ = true;
    return iter->second;
  }

  inline void ProcessCmdLine::showLogFile()
  {
    *pOut_ << logFile() << " ";
  }

  /*----< parse operations >-----------------------------------------*/

  inline bool ProcessCmdLine::parseError()
  {
    return parseError_;
  }

  inline void ProcessCmdLine::showParse()
  {
    *pOut_ << "\n  ";
    if (path() != "")
    {
      *pOut_ << "\n  Path:     ";
      showPath();
    }
    *pOut_ << "\n  options:  ";
    preface("", false);
    showOptions();
    *pOut_ << "\n  patterns: ";
    preface("", false);
    showPatterns();
    if (regex() != "")
    {
      *pOut_ << "\n  Regex:    ";
      preface("", false);
      showRegex();
    }
    if (logFile() != "")
    {
      *pOut_ << "\n  LogFile:  ";
      preface("", false);
      showLogFile();
    }
    if (maxItems() > 0)
    {
      *pOut_ << "\n  maxItems: ";
      preface("", false);
      showMaxItems();
    }
    *pOut_ << "\n";
  }

  /*----< command line operations >----------------------------------*/

  inline void ProcessCmdLine::process(int argc, char** argv)
  {
    argc_ = argc;
    for (int i = 0; i < argc; ++i)
    {
      argv_.push_back(argv[i]);
    }
    for (int i = 0; i < argc; ++i)
    {
      char* ptr = argv[i];
      argv_[i] = argv[i];
    }
    process();
  }

  inline void ProcessCmdLine::process()
  {
    if (msg_.str() == "")
      defaultUsageMessage();

    size_t i = 1;
    while (i < argc_)
    {
      if (argv_[i][0] == '/')
      {
        options_[argv_[i][1]] = "";
      }
      else
      {
        options_[argv_[i - 1][1]] = argv_[i];
      }
      ++i;
    }
    Patterns splits;
    for (auto optItem : options_)
    {
      switch (optItem.first)
      {
      case 'P':
        if (options_['P'] == "")
          parseError_ = true;
        break;
      case 'p':
        splits = split(optItem.second, ',');
        for (auto patt : splits)
          patterns_.push_back(patt);
        break;
      case 'n':
        if (options_['n'] == "")
          parseError_ = true;
        break;
      case 'R':
        if (options_['R'] == "")
          parseError_ = true;
        break;
      case 'F':
        if (options_['F'] == "")
          parseError_ = true;
        break;
      case 'h':
        usage();
        break;
      default:
        break;
      }
    }

    if (path() == "")
      path(".");
    if (patterns_.size() == 0)
      patterns_.push_back("*.*");
  }

  inline ProcessCmdLine::ProcessCmdLine(int argc, char** argv, std::ostream& out)
    : argc_(argc), argv_(std::vector<char*>(argv, argv+argc)), pOut_(&out) {}

  inline void ProcessCmdLine::showCmdLine(int argc, char** argv, bool showFirst)
  {
    *pOut_ << "\n\n  commandline args: ";
    
    if (showFirst)
      * pOut_ << "\n  " << argv[0] << " ";
    else
      *pOut_ << "\n  ";

    for (int i = 1; i < argc; ++i)
    {
      *pOut_ << argv[i] << " ";
    }
  }

  inline void ProcessCmdLine::defaultUsageMessage()
  {
    msg_ << "\n  Command Line: [/opt arg]* [/opt]*";
    msg_ << "\n    /opt arg has option type - a character, and option argument, a literal string";
    msg_ << "\n    Examples:";
    msg_ << "\n      /P \"../..\"             // starting path";
    msg_ << "\n      /p \"*.h,*.cpp,*.cs\"    // file patterns - no spaces";
    msg_ << "\n      /R \"threads|sockets\"   // regular expression";
    msg_ << "\n      /F \"logFile.txt\"       // log file";
    msg_ << "\n      /n \"42\"                // max items";
    msg_ << "\n    /option has option type with no argument";
    msg_ << "\n    Examples:";
    msg_ << "\n      /s                       // recurse";
    msg_ << "\n      /f                       // process files";
    msg_ << "\n      /d                       // process directories";
    msg_ << "\n";
  }

  inline void ProcessCmdLine::setUsageMessage(const std::string& msg)
  {
    msg_ << msg;
  }

  inline void ProcessCmdLine::usage(const std::string& msg)
  {
    if (msg == "")
      *pOut_ << msg_.str();
    else
      msg_.str(msg);
  }

  /////////////////////////////////////////////////////////////////////
  // Converter class
  // - supports converting unspecified types to and from strings
  // - a type is convertible if it provides insertion and extraction
  //   operators

  template <typename T>
  class Converter
  {
  public:
    static std::string toString(const T& t);
    static T toValue(const std::string& src);
  };
  //----< convert t to a string >--------------------------------------

  template <typename T>
  std::string Converter<T>::toString(const T& t)
  {
    std::ostringstream out;
    out << t;
    return out.str();
  }
  //----< convert a string to an instance of T >-----------------------
  /*
  *  - the string must have been generated by Converter<T>::toString(const T& t)
  *  - T::operator>> must be the inverse of T::operator<< 
  */
  template<typename T>
  T Converter<T>::toValue(const std::string& src)
  {
    std::istringstream in(src);
    T t;
    in >> t;
    return t;
  }

  ///////////////////////////////////////////////////////////////////////
  // Box class
  // - wraps primitive type in class
  // - preserves primitive syntax

  template<typename T>
  class Box
  {
  public:
    Box() : primitive_(T()) {}
    Box(const T& t) : primitive_(t) {}
    operator T&() { return primitive_; }
    T& operator=(const T& t) { primitive_ = t; return primitive_; }
  private:
    T primitive_;
  };

  ///////////////////////////////////////////////////////////////////////
  // ToXml interface
  // - defines language for creating XML elements

  struct ToXml
  {
    virtual std::string toXml(const std::string& tag) = 0;
    virtual ~ToXml() {};
  };

  ///////////////////////////////////////////////////////////////////////
  // PersistFactory<T> class
  // - wraps an instance of user-defined type
  // - preserves semantics of user-defined type
  // - adds toXml("tag") method

  template<typename T>
  class PersistFactory : public T, ToXml
  {
  public:
    PersistFactory() = default;
    PersistFactory(const T& t)
    {
      T::operator=(t);
    }
    std::string toXml(const std::string& tag)
    {
      std::ostringstream out;
      out << "<" << tag << ">" << *this << "</" << tag << ">";
      return out.str();
    }
  };
}