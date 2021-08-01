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

#include <cmath>
#include <limits>
#include "mpi.h"

#define PRINT_PRIMES {                                      \
  if(ALG_ERATSIEVE_DEBUG) {                                 \
    for (auto prime : *curPrimes) std::cout << prime << ' ';  \
  }                                                           \
  }

using namespace std;
using namespace Utils;

namespace Alg {

eratSieve::eratSieve(const cacheInfo* cinfo, 
                     const unsigned userRightLim,
                     vector<primeT>* curPrimes)
  : cinfo(cinfo), userRightLim(userRightLim),
    markWindow(num<unsigned>::min(userRightLim + 1, 
                                  cinfo->size * 4)), // cache / 2
    curPrimes(curPrimes),
    numMarkedElems(0), windowLeftLim(0), markedElemsLeftLim(3),
    numPrimesInFirstWindow(0)
{
  try {
    LOG(ALG_ERATSIEVE_DEBUG, "(eratSieve) Start Constructor");
    initMPIVariables();

    initCurPrimes();
    // We trust that userRightLim >= 2, which is controlled by the
    // calling procedure.
    markWindow[0] = 1;
    markWindow[1] = 1;
    markWindow[2] = 1;
    numMarkedElems = 3;

    firstPass(); // Get a lot of primes. This makes the process 
                 //   much quicker.


    // If the user right lim is lesser, we don't even need to call
    // the following procedure.
    if (userRightLim > windowLeftLim) {
      markPrimesLocal();
    }
  }
  catch (std::exception& e) {
    destroy();
    throw;
  }  

  LOG(ALG_ERATSIEVE_DEBUG, "(eratSieve) End Constructor");
}

eratSieve::~eratSieve()
{
  destroy();
}


void eratSieve::initMPIVariables()
{
  MPI_Comm_rank(MPI_COMM_WORLD, &myProcRank);
  MPI_Comm_size(MPI_COMM_WORLD, &commSz);
  
  // cinfo->size is in bytes.
  interProcBusWidth = cinfo->size / (1.2 * sizeof(primeT));
}

void eratSieve::initCurPrimes()
{
  // Allocate a good amount of memory for the vector
  unsigned numPrimesEstimate = userRightLim / log(userRightLim);
  curPrimes->reserve(num<primeT>::max(numPrimesEstimate,
                                     0xFFFFFF));

  curPrimes->push_back(2); // Start with 2 as the only prime
}

void eratSieve::destroy()
{
  // Put here the destructor's implementation
//  PRINT_PRIMES
}


void eratSieve::firstPass()
{
  // Do a first pass in all processes asynchronously. In terms of
  //   execution time, this should be much faster.
  //
  // The point is that this gives us a good prime number to start
  // with, fetched by getMaxPrimeLocal()
  findPrimesBetween(3, markWindow.size());
  numPrimesInFirstWindow = curPrimes->size();
}

void eratSieve::markPrimesLocal()
{
  LOG(ALG_ERATSIEVE_DEBUG, "P%d In markPrimesLocal", myProcRank);
  LOG(ALG_ERATSIEVE_DEBUG, "P%d windowLeftLim: %d", myProcRank, 
      windowLeftLim);
  // Calculate the left limit for the process
  unsigned myLLimit = getLLimit();
  unsigned myRLimit = getRLimit();

  LOG(ALG_ERATSIEVE_DEBUG, "P%d myLLlimit, myRLimit: %u, %u", 
      myProcRank, myLLimit, myRLimit);

  // Fixme: add loop

  windowLeftLim = markedElemsLeftLim = myLLimit;
 
  findPrimesBetween(myLLimit, myRLimit);
  
  fuseCurPrimesGlobal(myLLimit, myRLimit);

  LOG(ALG_ERATSIEVE_DEBUG, "P%d Out markPrimesLocal", myProcRank);
}


void eratSieve::findPrimesBetween(const unsigned leftLim,
                                  const unsigned rightLim)
{
  // Go through the bitset, each prime at a time, marking their
  // multiples as non-prime.
  //
  // End when the counter equals the size of the vector, or we have
  // gone through all the primes we have.

  // Walk by blocks of size of markWindow->size().
  // Notice that windowLeftLim != markedElemsLeftLim
  for (markedElemsLeftLim = leftLim;
       markedElemsLeftLim < rightLim;
       windowLeftLim += markWindow.size(),
         markedElemsLeftLim = windowLeftLim) {

    primeT curPrime = 0;
    for (auto lit = curPrimes->begin(); 
         numMarkedElems < markWindow.size()
           && markedElemsLeftLim < rightLim;
         lit++) {

      if ((*lit) * (*lit) > windowLeftLim + markWindow.size()) {
        // Everything unmarked is a prime!        
        break;
      }

      // If we went over all the primes, and there are numbers that
      // were not marked, the first one of these is a prime, and we
      // should continue our search
      moveLeftMarkToRight();
      curPrimes->push_back(markedElemsLeftLim);
      markPrimeAt(markedElemsLeftLim % markWindow.size());

      // Choose the prime, and then walk the vector marking its
      // multiples as non-primes
      
      curPrime = *lit;

      // Example:
      // curPrime = 3931
      // windowLeftLimit = 12000
      // markWindow.size() = 4000
      // (12000 + 3931 - 69) % 4000 = (2 * 3931) % 4000 = 3862     
      //
      // Current prime multiple
      primeT curPrimeMul = (curPrime - (windowLeftLim % curPrime))
        % markWindow.size();
      for (;
           curPrimeMul < markWindow.size();
           curPrimeMul += curPrime) {
        markPrimeAt(curPrimeMul);
      }
    }
    allUnmarkedArePrimes(rightLim);
  }

}

void eratSieve::fuseCurPrimesGlobal(const unsigned myLeftLim,
                                    const unsigned myRightLim)
{
  if (myProcRank == 0) {
    // How many primes should I receive?
    // Create vector with sizes of receives
    unsigned numReceives = (commSz - 1);

    MPI_Status status;    
    // Receive prime arrays
    primeT* primeArr = new primeT [interProcBusWidth];
    for (unsigned i = 0; i < numReceives; ++i) {
      
      unsigned recvSz;
      MPI_Recv(&recvSz, 1, MPI::UNSIGNED, i + 1, 0, MPI_COMM_WORLD,
               &status);

      int numPrimesNotRcvd = recvSz;
      for (; numPrimesNotRcvd > 0; 
           numPrimesNotRcvd -= interProcBusWidth) {
        MPI_Recv(primeArr, 
                 num<unsigned>::min(recvSz, interProcBusWidth),
                 MPI::UNSIGNED, i + 1, 0, MPI_COMM_WORLD, &status);

        // FIXME: this copy can be made quite more efficient
        for (unsigned ii = 0; 
             ii < num<primeT>::min(status._ucount / sizeof(primeT), 
                                          numPrimesNotRcvd);
             ++ii) {
          curPrimes->push_back(primeArr[ii]);
        }
      }
    }

    delete[] primeArr;
  }
  else {
    unsigned size = curPrimes->size() - numPrimesInFirstWindow;
    MPI_Send(&size, 1, MPI::UNSIGNED, 0, 0, MPI_COMM_WORLD);

    int szDecounter = 0;
    unsigned i = 0;
    // Send information in blocks
    for (szDecounter = size, i = 0; szDecounter > 0; 
         szDecounter -= interProcBusWidth,
         i += interProcBusWidth) {
      MPI_Send(&(*curPrimes)[i + numPrimesInFirstWindow],
               Utils::num<unsigned>::min(szDecounter, 
                                         interProcBusWidth),
               MPI::UNSIGNED, 0, 0, 
               MPI_COMM_WORLD);
    }
  }

}

}
