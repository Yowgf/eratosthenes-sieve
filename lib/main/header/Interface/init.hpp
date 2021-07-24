//===----------------------------------------------------------===//
// Interface module
//
// File purpose: init class header
//
// Description: this class validates user argument and gets it ready
// to be run on the target algorithm
//===----------------------------------------------------------===//

#ifndef INIT_H
#define INIT_H

#include "Alg/eratSieve.hpp"
#include "DS/array.hpp"
#include "Utils/defs.hpp"
#include "Utils/hwInfo.hpp"

#include <fstream>
#include <list>

namespace Interface {

class init {
public:
  init(int argc, char** argv);
  ~init();
private:

  void destroy(); // Destructor

  // See ~validateArguments~ for details on how program arguments
  // are validated.
  const int knumProgArgs = 3;
  const int kminRightLim = 2;
  const int kmaxRightLim = 1e9;

  // Program entries
  int arrRightLim;
  char outMode;


  // processEntries build this object for the algorithm.
  Utils::cacheInfo cinfo;

  // Performs some basic validation on the program arguments.
  //
  // Program should receive exactly two arguments:
  //
  // - The right limit (~n~) for the vector of numbers we will
  // create (limit goes from [2, n]). 2 <= n <= 1e9
  //
  // - The mode of output:
  //
  //   - l: print list of primes until n.
  //   - t: print time of execution (6 decimal places).
  //   - a: all (l and t)
  void setAndValidateArguments(int argc, char** argv)
    noexcept(false);

  // No validation is needed here. Just build the entry array.
  void processEntries(int argc, char** argv) noexcept(false);

  //===--------------------------------------------------------===//
  // Output stuff
  //===--------------------------------------------------------===//
  // Decided according to outMode in processEntries
  bool shouldPrintList;
  bool shouldPrintTime;

  // We pass this as an argument to the algorithm, and let it take
  // care of the rest.
  std::list<primeT> primesList;

  // Prints output, according to outMode
  void printOutput();
  void printOutList();
  void printOutTime();
};

}

#endif
