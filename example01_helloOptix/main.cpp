#include "optix7.h"
#include <iostream>

#ifndef TERMINAL_COLORS
#define TERMINAL_COLORS
#define TERMINAL_RED "\033[1;31m"
#define TERMINAL_GREEN "\033[1;32m"
#define TERMINAL_YELLOW "\033[1;33m"
#define TERMINAL_BLUE "\033[1;34m"
#define TERMINAL_RESET "\033[0m"
#define TERMINAL_DEFAULT TERMINAL_RESET
#define TERMINAL_BOLD "\033[1;1m"
#endif
#ifndef PRINT
# define PRINT(var) std::cout << #var << "=" << var << std::endl;
# define PING std::cout << __FILE__ << "::" << __LINE__ << ": " << __FUNCTION__ << std::endl;
#endif

/*! \namespace osc - Optix Siggraph Course */
namespace osc {

  /*! helper function that initializes optix and checks for errors */
  void initOptix()
  {
    // -------------------------------------------------------
    // check for available optix7 capable devices
    // -------------------------------------------------------
    cudaFree(0);
    int numDevices;
    cudaGetDeviceCount(&numDevices);
    if (numDevices == 0)
      throw std::runtime_error("no CUDA capable devices found!");
    std::cout << "found " << numDevices << " CUDA devices" << std::endl;

    // -------------------------------------------------------
    // initialize optix
    // -------------------------------------------------------
    OPTIX_CHECK( optixInit() );
  }

  
  /*! main entry point to this example - initially optix, print hello
    world, then exit */
  extern "C" int main(int ac, char **av)
  {
    try {
      std::cout << "initializing optix..." << std::endl;
      
      initOptix();
      
      std::cout << TERMINAL_GREEN
                << "successfully initialized optix... yay!"
                << TERMINAL_DEFAULT << std::endl;

      // for this simple hello-world example, don't do anything else
      // ...
      std::cout << "done. clean exit." << std::endl;
      
    } catch (std::runtime_error& e) {
      std::cout << TERMINAL_RED << "FATAL ERROR: " << e.what()
                << TERMINAL_DEFAULT << std::endl;
      exit(1);
    }
    return 0;
  }
  
}
