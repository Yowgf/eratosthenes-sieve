//===----------------------------------------------------------===//
// Alg module
//
// File purpose: implementation of the parallel, optimized version
// of the eratosthenes sieve algorithm.
//===----------------------------------------------------------===//

// TODO: heuristics to not have to compare all the primes in the
// list.

#include "Alg/eratSieve.hpp"
#include "Utils/error.hpp"
#include "Utils/num.hpp"

#include <limits>

using namespace std;
using namespace Utils;

namespace Alg {

eratSieve::eratSieve(const cacheInfo* cinfo, 
                     const unsigned arrRightLim,
                     list<primeT>& curPrimes)
  : arrRightLim(arrRightLim), 
    markVec(num<unsigned>::min(arrRightLim + 1, cinfo->size * 7)), 
    curPrimes(curPrimes),
    numMarkedElems(0), markedElemsLeftLim(3)
{
  try {
    curPrimes.push_front(2); // Start with 2 as the only prime
    // We trust that arrRightLim >= 2, which is controlled by the
    // calling procedure.
    markVec[0] = 1;
    markVec[1] = 1;
    markVec[2] = 1;
    numMarkedElems = 3;
    markPrimes();
  }
  catch (std::exception& e) {
    destroy();
    throw;
  }  
}

void eratSieve::destroy()
{
  // Put here the destructor's implementation
}

eratSieve::~eratSieve()
{
  destroy();
}

void eratSieve::markPrimes()
{
  // Go through the boolean vector, each prime at a time, marking
  // their multiples as non-prime.
  //
  // End when the counter equals the size of the vector, or we have
  // gone through all the primes we have.

  // Walk by blocks of size of markVec->size().
  // Notice that vecLeftLim != markedElemsLeftLim
  primeT vecLeftLim = 0;
  for (; 
       markedElemsLeftLim < arrRightLim;
       vecLeftLim += markVec.size(),
         markedElemsLeftLim = vecLeftLim) {

    primeT curPrime = 0;
    for (auto lit = curPrimes.begin(); 
         numMarkedElems < markVec.size()
           && markedElemsLeftLim < arrRightLim;
         lit++) {
      // If we went over all the primes, and there are numbers that
      // were not marked, the first one of these is a prime, and we
      // should continue our search

      if (lit != curPrimes.end()) {
        if ((*lit) * (*lit) > vecLeftLim + markVec.size()) {
          // Everything unmarked is a prime!
          break;
        }
      }
      else {
        moveLeftMarkToRight();
        curPrimes.push_back(markedElemsLeftLim);
        markPrimeAt(markedElemsLeftLim % markVec.size());
        lit--;
      }

      // Choose the prime, and then walk the vector marking its
      // multiples as non-primes
      curPrime = *lit;

      // Example:
      // curPrime = 3931
      // vecLeftLimit = 12000
      // markVec.size() = 4000
      // (12000 + 3931 - 69) % 4000 = (2 * 3931) % 4000 = 3862     
      //
      // Current prime multiple
      // TODO: optimization: can always start at curPrime ^ 2
      primeT curPrimeMul = (vecLeftLim + curPrime - 
        (vecLeftLim % curPrime)) % markVec.size();

      for (; 
           curPrimeMul < markVec.size();
           curPrimeMul += curPrime) {
        markPrimeAt(curPrimeMul);
      }
    }
    
    allUnmarkedArePrimes(vecLeftLim);
  }
}

}
