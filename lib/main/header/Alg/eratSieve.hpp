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
#include "Utils/hwInfo.hpp"

namespace Alg {

class eratSieve {
public:
  eratSieve(const Utils::cacheInfo*);
  ~eratSieve();
};

}

#endif
