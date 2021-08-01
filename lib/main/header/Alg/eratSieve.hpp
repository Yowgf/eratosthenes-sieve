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
#include "Utils/num.hpp"


#include <boost/dynamic_bitset.hpp>

#include <vector>

#define SQRD(n) (n > 65535 ? 4294967295 : (n) * (n))


typedef unsigned primeT;

namespace Alg {

class eratSieve {
public:
  eratSieve(const Utils::cacheInfo*, const unsigned userRightLim,
            std::vector<primeT>* curPrimes);
  ~eratSieve();

private:
  // Input constants
  const Utils::cacheInfo* cinfo;
  const unsigned userRightLim;

  // MPI variables
  int myProcRank;
  int commSz;
  unsigned interProcBusWidth;


  //===--------------------------------------------------------===//
  // Aux. procedures
  //===--------------------------------------------------------===//
  void initMPIVariables();
  void initCurPrimes();
  void destroy();

  //===--------------------------------------------------------===//
  // Objects used by the algorithm.
  //===--------------------------------------------------------===//

  // Numbers currently marked as primes. Allocated according to size
  // of the cache line.
  //
  // This bitset is updated all the time.
  boost::dynamic_bitset<> markWindow;
  // Current list of primes.
  std::vector<primeT>* curPrimes;

  // Counter of how many elements are marked (i.e. bit set to 1) in
  // markWindow.
  unsigned numMarkedElems;

  unsigned windowLeftLim;

  // First number that has not been marked, in markWindow
  primeT markedElemsLeftLim;

  // This is used to broadcast the sizes of each process later on,
  // in fuseCurPrimesGlobal.
  unsigned numPrimesInFirstWindow;

  //===--------------------------------------------------------===//
  // Procedures actually used by the algorithm.
  //===--------------------------------------------------------===//
  void firstPass();
  void markPrimesLocal();
  void findPrimesBetween(const unsigned leftLim,
                         const unsigned rightLim);
  void fuseCurPrimesGlobal(const unsigned myLeftLim,
                           const unsigned myRightLim);

  inline unsigned getLLimit() const
  {
    const unsigned minLLBase = 
      Utils::num<unsigned>::min(SQRD(getMaxPrime()), userRightLim);
    return windowLeftLim + 
      myProcRank * ceil((minLLBase - windowLeftLim) / 
                        (float) commSz);
  }

  inline unsigned getRLimit() const
  {
    const unsigned minRLBase = 
      Utils::num<unsigned>::min(SQRD(getMaxPrime()), userRightLim);
    const unsigned distFromMinBase = minRLBase - windowLeftLim;

    if (myProcRank == commSz - 1) {
      return minRLBase;
    }

    return windowLeftLim + (myProcRank + 1) *
      floor((minRLBase - windowLeftLim) / (float) commSz);
  }

  inline primeT getMaxPrime() const
  {
    return curPrimes->back();
  }

  inline void markPrimeAt(const unsigned pos)
  {
    if (!markWindow[pos]) {
      markWindow[pos] = 1;
      ++numMarkedElems;
    }
  }

  inline void moveLeftMarkToRight()
  {
    while (markWindow[markedElemsLeftLim % markWindow.size()]) {
      ++markedElemsLeftLim;
    }
  }

  // Marks everything and adds it to the curPrimes list!
  inline void allUnmarkedArePrimes(const unsigned myRightLim)
  {
    for (; markedElemsLeftLim < windowLeftLim + markWindow.size()
           && markedElemsLeftLim < myRightLim;
         ++markedElemsLeftLim) {
      if (!markWindow[markedElemsLeftLim - windowLeftLim]) {
        ++numMarkedElems;           
        // Don't even need to set markWindow[...] = 1, since the
        // vector is resetted just after
        curPrimes->push_back(markedElemsLeftLim);
      }        
    }
    resetMarkWindow();
  }

  inline void resetMarkWindow()
  {
    markWindow.reset();
    numMarkedElems = 0;
  }
};

}

#undef SQRD
#endif
