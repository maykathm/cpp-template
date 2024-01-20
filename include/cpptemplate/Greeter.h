#ifndef CPPTEMPLATE_GREETER_H
#define CPPTEMPLATE_GREETER_H

#include <string>
#include "api.h"
#include <mpi.h>
#include <vector>

namespace cpptemplate {
  
  class DLL_PUBLIC Greeter {
  public:
    std::string sayHi(const std::string& name);
    std::string sayHi(const std::string& name, MPI_Comm comm);
    std::vector<float> copy(const std::vector<float>& vector);
  };

}

#endif