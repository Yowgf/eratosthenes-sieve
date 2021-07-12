//===----------------------------------------------------------===//
// Interface module
//
// File purpose: init class implementation. See class header for
// more detail.
//===----------------------------------------------------------===//

#include "Interface/init.hpp"
#include "Utils/error.hpp"
#include "Utils/file.hpp"
#include "Utils/num.hpp"

#include <iostream>
#include <stdexcept>

using namespace Utils;
using namespace std;

namespace Interface {

init::init(int argc, char** argv) : twoToN(nullptr)
{
  validateArguments(argc, argv);
  processEntries(argc, argv);
}

init::~init()
{
  destroy();
}

void init::destroy()
{
  delete twoToN;
}

void init::validateArguments(int argc, char** argv) 
  const noexcept(false)
{
  if (argc != knumProgArgs) {
    throw std::invalid_argument {
      "Wrong number of arguments.\n"\
        "Program usage:\n"\
        "<program> <array-right-limit> (l | t | a)"};
  }

  const int arrRightLim = atoi(argv[1]);
  num<int>::checkInRange(arrRightLim, kminRightLim, kmaxRightLim);

  const char outMode = argv[2][0];
  switch(outMode) {
    case 'l':
    case 't':
    case 'a':
      break;
    default:
      throw std::invalid_argument {
        string("Invalid output mode '") + argv[2] + '\''};
  }

}

void init::processEntries(int argc, char** argv) noexcept(false)
{
  
}

}
