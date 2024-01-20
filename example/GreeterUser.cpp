#include <cstdlib>
#include "cpptemplate/Greeter.h"
#include <iostream>
#include <numeric>
#include <mpi.h>

int main(int argc, char* argv[]) {
  cpptemplate::Greeter greeter;
  auto greeting = greeter.sayHi("Greeter User");
  std::cout << "The greeter says: " << greeting << std::endl;

  std::vector<float> coolVector(1000);
  std::iota(begin(coolVector), end(coolVector), 11);
  auto coolVectorCopy = greeter.copy(coolVector);

  MPI_Init(&argc, &argv);

  auto mpiGreetings = greeter.sayHi("MPI user", MPI_COMM_WORLD);
  std::cout << "The mpi greeter says: " << mpiGreetings << std::endl;

  MPI_Finalize();
  return EXIT_SUCCESS;
}