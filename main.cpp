#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "cli/cli.hpp"
#include "core/attacker.hpp"

int main(int argc, char* argv[])
{
  try{
    ArgumentsProcessor processor(argc, argv);

    TargetInfo target = processor.process();

    std::cout << "--------------------------------------------\n"
        << "Project initialized successfully.\n"
        << "Target IP:   " << target.ip << std::endl
        << "Target Port: " << target.port << std::endl
        << "--------------------------------------------\n";
    CoreAttacker attacker(target);
    attacker.start_flood();
    
  }
  catch(const std::exception& e){
    std::cerr << "FATAL SYSTEM ERROR: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
