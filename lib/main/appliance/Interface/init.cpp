//===----------------------------------------------------------===//
// Interface module
//
// File purpose: init class implementation. See class header for
// more detail.
//===----------------------------------------------------------===//

#include "Interface/init.hpp"

#include "Alg/eratSieve.hpp"
#include "Utils/error.hpp"
#include "Utils/file.hpp"
#include "Utils/num.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>

using namespace Utils;
using namespace std;

namespace Interface {

init::init(int argc, char** argv) 
  : arrRightLim(0), outMode('\0'), shouldPrintList(false), 
    shouldPrintTime(false), clkVar(0)
{
  setAndValidateArguments(argc, argv);
  processEntries(argc, argv);

  TIME_EXECUTION(clkVar, Alg::eratSieve(&cinfo, static_cast<unsigned>(arrRightLim), primesList));
}

init::~init()
{
  destroy();
}

void init::destroy()
{
  printOutput();
}

void init::setAndValidateArguments(int argc, char** argv) 
  noexcept(false)
{
  if (argc != knumProgArgs) {
    throw std::invalid_argument {
      "Wrong number of arguments.\n"\
        "Program usage:\n"\
        "<program> <array-right-limit> (l | t | a)"};
  }

  arrRightLim = atoi(argv[1]);
  num<int>::checkInRange(arrRightLim, kminRightLim, kmaxRightLim);

  outMode = argv[2][0];
  switch(outMode) {
    case 'l': // List
    case 't': // Time
    case 'a': // All
      break;
    default:
      throw std::invalid_argument {
        string("Invalid output mode '") + argv[2] + '\''};
  }
}

void init::processEntries(int argc, char** argv) noexcept(false)
{
  Utils::hwInfo::fetchCacheInfo(&cinfo, LEVEL1, DATA_CACHE);

  switch(outMode) {
    case 'l': // List
      shouldPrintList = true;
      break;
    case 't': // Time
      shouldPrintTime = true;
      break;
    case 'a': // All
      shouldPrintList = shouldPrintTime = true;
      break;
  }
}

void init::printOutput()
{
  if (shouldPrintList) {
    printOutList();
  }
  if (shouldPrintTime) {
    printOutTime();
  }
}

void init::printOutList()
{
  for (auto prime : primesList) {
    cout << prime << ' ';
  }
  cout << '\n';
}

void init::printOutTime()
{
  cout << setprecision(6) << fixed << clkVar.count() << '\n';
}

}
