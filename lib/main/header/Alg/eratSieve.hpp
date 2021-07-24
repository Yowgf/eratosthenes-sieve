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
    LOG(1, "M%d", markedElemsLeftLim);
    // Move markedElemsLeftLim to the right as far as possible
    while (markVec[markedElemsLeftLim % markVec.size()] 
           && markedElemsLeftLim < markVec.size() - 1) {
      ++markedElemsLeftLim;
    }
  }

  inline void resetMarkVec()
  {
    LOG(1, "Resetting vector");
    numMarkedElems = 0;
    markVec.reset();
  }
};

}

#endif
