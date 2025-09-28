#ifndef CLI_HPP
#define CLI_HPP
#include <iostream>
#include <string>

struct TargetInfo{
  std::string ip;
    int port = 0;
};

class ArgumentsProcessor{
  public:
    
    ArgumentsProcessor(int argc, char** argv):
        argc_(argc), argv_(argv), programName(argv[0]) {}

    TargetInfo process() const;


  private:

      const int argc_;
      char** argv_;
      std::string programName;

      void print_help() const;
      void exit_with_error(const std::string& msg) const;
};

#endif 
