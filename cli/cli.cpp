#include "cli.hpp"
#include <iostream>
#include <stdexcept>
#include <arpa/inet.h>
#include "../utils/dns.hpp"
#include "../utils/net_utils.hpp"
#include <limits>
#include <vector>
#include <string>
#include <algorithm>

void ArgumentsProcessor::print_help() const
{
  std::cout << "Usage: " << this->programName << " [options]\n\n"
    << "Options:\n"
    << "  -h, --help    Show this help message.\n"
    << "  -i, --ip      <ip_address>  Specify target IP address or hostname.\n"
    << "  -p, --port    <port>        Specify target port.\n"
    << "  -if, --interface <name>     Specify network interface (e.g., eth0).\n"
    << "  -t, --threads <number>      Specify number of threads (Default: 100).\n";
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
        std::string input_host = this->argv_[++i];

        if (inet_addr(input_host.c_str()) != INADDR_NONE) {
            info.ip = input_host; 
        } else {
            try {
                info.ip = Utils::resolve_hostname(input_host);
                std::cout << "[INFO] Resolved " << input_host << " to IP: " << info.ip << std::endl;
            } catch (const std::exception& e) {
                this->exit_with_error(e.what()); 
            }
        }
      } else {
        this->exit_with_error("Missing IP address or hostname after " + arg);
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

    else if(arg == "-if" || arg == "--interface"){ 
      if( i + 1 < this->argc_ ){
        info.interface = this->argv_[++i];
      }else{
        this->exit_with_error("Missing interface name after " + arg);
      }
    }
    
    else if(arg == "-t" || arg == "--threads"){ 
      if( i + 1 < this->argc_ ){
        try{
            info.threads_count = std::stoi(this->argv_[++i]);
            if (info.threads_count <= 0) {
                 this->exit_with_error("Thread count must be greater than 0.");
            }
        }catch (const std::exception& e){
            this->exit_with_error("Invalid thread count: " + std::string(this->argv_[i]));
        }
      }else{
        this->exit_with_error("Missing thread count after " + arg);
      }
    }

    else{
        this->exit_with_error("Unknown argument: "+arg);
    }

  }
  
  if (info.ip.empty() || info.port == 0) {
      this->exit_with_error("Missing required IP/Hostname or Port argument.");
  }
  
  return info;
}

TargetInfo ArgumentsProcessor::interactive_mode() const {
    TargetInfo info;
    std::string ip_or_host;
    info.threads_count = 100;

    std::cout << "\n\n\033[31m";
    std::cout << "--------------------------------------------------------\n";
    std::cout << "This tool is for educational purposes only.\n";
    std::cout << "It demonstrates the construction and sending of raw TCP SYN packets.\n";
    std::cout << "Use responsibly and only on networks or hosts where you have explicit permission.\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << "\033[0m\n\n";
    do {
        std::cout << "Enter target IP or Hostname (e.g., example.com): ";
        std::getline(std::cin, ip_or_host);

        if (ip_or_host.empty()) {
            std::cout << "Target cannot be empty. Please try again.\n";
            continue;
        }

        if (inet_addr(ip_or_host.c_str()) != INADDR_NONE) {
            info.ip = ip_or_host;
        } else {
            try {
                info.ip = Utils::resolve_hostname(ip_or_host);
                std::cout << "-> Resolved " << ip_or_host << " to IP: " << info.ip << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "-> Error: DNS resolution failed. " << e.what() << ". Try again.\n";
                info.ip = ""; 
            }
        }
    } while (info.ip.empty());

    do {
        std::cout << "Enter target port (e.g., 80, 443): ";
        if (!(std::cin >> info.port) || info.port <= 0 || info.port > 65535) {
            std::cerr << "Invalid port number. Try again.\n";
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            info.port = 0;
        }
    } while (info.port == 0);

    std::cout << "\n--- Interface Selection ---\n";
    std::vector<std::string> available_interfaces = Utils::get_all_interfaces();
    info.interface = ""; 

    if (!available_interfaces.empty()) {
        std::cout << "Available Interfaces:\n";
        for (size_t i = 0; i < available_interfaces.size(); ++i) {
            std::cout << "  [" << i + 1 << "] " << available_interfaces[i] << "\n";
        }
        
        size_t choice = 0;
        std::cout << "Select interface number (or 0 to skip): ";
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        
        if (std::cin >> choice && choice > 0 && choice <= available_interfaces.size()) {
            info.interface = available_interfaces[choice - 1];
            std::cout << "-> Interface selected: " << info.interface << std::endl;
        } else if (choice == 0) {
            std::cout << "-> Interface selection skipped.\n";
        } else {
            std::cout << "-> Invalid choice or input. Skipping interface binding.\n";
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    } else {
        std::cout << "Warning: Could not list interfaces. Interface binding will be skipped.\n";
    }

    std::string threads_input;
    std::cout << "\nEnter number of threads (Default = " << info.threads_count << "): ";
    std::getline(std::cin, threads_input);

    if (!threads_input.empty()) {
        try {
            int new_threads = std::stoi(threads_input);
            if (new_threads > 0) {
                info.threads_count = new_threads;
            }
        } catch (...) {
            std::cout << "-> Invalid input. Using Default: " << info.threads_count << " threads.\n";
        }
    }
    
    std::cout << "\n--- Interactive Setup Complete ---\n";
    return info;
}
