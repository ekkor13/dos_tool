#include "cli.hpp"
#include <iostream>
#include <stdexcept>

void ArgumentsProcessor::print_help() const
{
  std::cout << "Usage: " << this->programName << " [options]\n\n"
    << "Options:\n"
    << "  -h, --help    Show this help message.\n"
    << "  -i, --ip      <ip_address>  Specify target IP address.\n"
    << "  -p, --port    <port>        Specify target port.\n";
}

void ArgumentsProcessor::exit_with_error(const std::string& msg) const
{
  std::cerr << "Error: " << msg << std::endl;
  this->print_help();
  exit(1);
}

TargetInfo ArgumentsProcessor::process() const
{
  TargetInfo info;

  if(this->argc_ < 2){
    this->exit_with_error("No arguments.");
  }

  for(int i = 1; i < this->argc_; ++i){
    std::string arg = this->argv_[i];

    if (arg == "-h" || arg == "--help"){
      this->print_help();
      exit(0);
    }

    else if(arg == "-i" || arg == "--ip"){
      if( i + 1 < this->argc_ ){
        info.ip = this->argv_[++i];
      }else{
        this->exit_with_error("Missing IP address after " + arg);
      }
    }
    
    else if(arg == "-p" || arg == "--port"){
      if(i + 1 < this-> argc_){
        try{
          info.port = std::stoi(this->argv_[++i]);
        }catch (const std::exception& e){
          this->exit_with_error("Invalid port number: " + std::string(this->argv_[i]));
        }
      }else{
        this->exit_with_error("Missing port number after: " + arg);
      }
    }

      else{
        this->exit_with_error("Unknown argument: "+arg);
      }

    
  }
  return info;
}
