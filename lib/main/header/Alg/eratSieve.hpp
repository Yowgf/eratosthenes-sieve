//===----------------------------------------------------------===//
// Alg module
//
// File purpose: declarations for the eratSieve (eratosthenes sieve)
// algorithm. This algorithm is pretty well-known, but here we
// implement an opmtimized, parallel version, that should run much
// faster than the naive version.
//===----------------------------------------------------------===//

#ifndef ERATSIEVE_H
#define ERATSIEVE_H

#include "Alg/eratSieve.hpp"
#include "Utils/error.hpp"
#include "Utils/hwInfo.hpp"

#include <boost/dynamic_bitset.hpp>

#include <list>


typedef unsigned primeT;

namespace Alg {

class eratSieve {
public:
  eratSieve(const Utils::cacheInfo*, const unsigned arrRightLim,
            std::list<primeT>& curPrimes);
  ~eratSieve();

private:
  const unsigned arrRightLim;

  // Numbers currently marked as primes. Allocated with size of the
  // cache line.
  //
  // This bitset is updated all the time.
  boost::dynamic_bitset<> markVec;
  // Current list of primes.
  std::list<primeT>& curPrimes;

  // Counter of how many elements are marked in markVec (bit set to
  // 1)
  unsigned numMarkedElems;

  // First number that has not been marked, in markVec
  primeT markedElemsLeftLim;


  //===--------------------------------------------------------===//
  // Memory safety functions.
  //===--------------------------------------------------------===//
  void destroy();

  //===--------------------------------------------------------===//
  // Procedures actually used by the algorithm.
  //===--------------------------------------------------------===//
  void markPrimes();
  
  inline void markPrimeAt(const unsigned pos)
  {
    if (!markVec[pos]) {
      markVec[pos] = 1;
      ++numMarkedElems;
    }
  }

  inline void moveLeftMarkToRight()
  {
    while (markVec[markedElemsLeftLim % markVec.size()]) {
      ++markedElemsLeftLim;
    }
  }


  // Marks everything and adds it to the curPrimes list!
  inline void allUnmarkedArePrimes(const unsigned vecLeftLim)
  {
    for (; markedElemsLeftLim < vecLeftLim + markVec.size();
         ++markedElemsLeftLim) {
      if (!markVec[markedElemsLeftLim % markVec.size()]) {
        ++numMarkedElems;                     
        // Don't even need to set markVec[...] = 1, since the
        // vectore is resetted just after
        curPrimes.push_back(markedElemsLeftLim);
      }        
    }
    resetMarkVec();
  }

  inline void resetMarkVec()
  {
    numMarkedElems = 0;
    markVec.reset();
  }
};

}

#endif
