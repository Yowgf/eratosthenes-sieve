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

using namespace std;
using namespace Utils;

namespace Alg {

eratSieve::eratSieve(const cacheInfo* cinfo, 
                     const unsigned arrRightLim,
                     list<primeT>& curPrimes)
  : arrRightLim(arrRightLim), 
    markVec(num<unsigned>::min(arrRightLim, cinfo->size * 7)), 
    curPrimes(curPrimes),
    numMarkedElems(0), markedElemsLeftLim(3)
{
  try {
    curPrimes.push_front(2); // Start with 2 as the only prime
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

  // Walk by blocks of size of markVec->size()

  primeT vecLeftLim = markedElemsLeftLim;
  for (; vecLeftLim < arrRightLim; vecLeftLim += markVec.size()) {

    markedElemsLeftLim = vecLeftLim;

    for (auto lit = curPrimes.begin(); 
         numMarkedElems < markVec.size();
         ++lit) {

      
      // If we went over all the primes, and there are numbers that
      // were not marked, the first one of these is a prime, and we
      // should continue our search
      if (lit == curPrimes.end()) {
        curPrimes.push_back(markedElemsLeftLim);
        moveLeftMarkToRight();
        markPrimeAt(markedElemsLeftLim % markVec.size());        
        lit--;
      }

      // Choose the prime, and then walk the vector marking its
      // multiples as non-primes
      LOG(1, "a");
      LOGATT(1, *lit);
      primeT curPrime = *lit;
      LOG(1, "b");
      // Example:
      // curPrime = 3931
      // vecLeftLimit = 12000
      // markVec.size() = 4000
      // (12000 + 3931 - 69) % 4000 = (2 * 3931) % 4000 = 3862     
      // Current prime multiple
      primeT curPrimeMul = vecLeftLim + curPrime - 
        (vecLeftLim % curPrime);
      if (curPrimeMul <= vecLeftLim + markVec.size()) {
        curPrimeMul %= markVec.size();
      }
      else {
        LOG(ALG_ERATSIEVE_DEBUG, "continuing");
        continue;
      }
      for (; 
           curPrimeMul < vecLeftLim + markVec.size();
           curPrimeMul += curPrime) {
        markPrimeAt(curPrimeMul);
      }
      
      LOGATT(ALG_ERATSIEVE_DEBUG, curPrimeMul);
      LOGATT(ALG_ERATSIEVE_DEBUG, numMarkedElems);
    }
    
    resetMarkVec();    
  }
}

}
