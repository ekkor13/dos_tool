#ifndef CLI_HPP
#define CLI_HPP

#include <iostream>
#include <string>

struct TargetInfo{
  std::string ip;
  int port = 0;
  std::string interface = "";
  int threads_count = 100;
};

class ArgumentsProcessor{
  public:
    
    ArgumentsProcessor(int argc, char** argv):
        argc_(argc), argv_(argv), programName(argv[0]) {}

    TargetInfo process() const;
    TargetInfo interactive_mode() const;

  private:

      const int argc_;
      char** argv_;
      std::string programName;

      void print_help() const;
      void exit_with_error(const std::string& msg) const;
};

#endif
