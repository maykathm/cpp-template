
#include "cpptemplate/Greeter.h"
#include <cblas.h>

namespace cpptemplate {

  std::string Greeter::sayHi(const std::string& name) {
    return "Hi, " + name;
  }

  std::string Greeter::sayHi(const std::string& name, MPI_Comm comm) {
    int rank;
    int status =  MPI_Comm_rank(comm, &rank);
    if (rank == 0)
      return "Hi, " + name;
    else
      return "You're not the first process!";
  }

  std::vector<float> Greeter::copy(const std::vector<float>& vector) {
      std::vector<float> vectorCopy(vector.size());
      cblas_scopy(vector.size(), vector.data(), 1, vectorCopy.data(), 1);
      return vectorCopy;
  }

}