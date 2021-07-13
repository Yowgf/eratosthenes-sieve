//===----------------------------------------------------------===//
// Utils module
//
// File purpose: declaration of hardware info related objects. By
// hardware info we mean functions that encapsulate the fetching of
// hardware-related information.
//===----------------------------------------------------------===//

#include "Utils/hwInfo.hpp"
#include "Utils/num.hpp"

#include "unistd.h"

namespace Utils {

void hwInfo::fetchCacheInfo(cacheInfo* const destStruct,
                            const unsigned level, 
                            const unsigned iOrC)
{  
  num<unsigned>::checkInRange(level, LEVEL1, LEVEL4);
  num<unsigned>::checkInRange(iOrC, INSTRUCTION_CACHE,
                              DATA_CACHE);
  switch (level) {
    case 1:
      switch (iOrC) {
        case INSTRUCTION_CACHE:
          destStruct->size = 
            sysconf(_SC_LEVEL1_ICACHE_SIZE);
          destStruct->waysOfAssoc = 
            sysconf(_SC_LEVEL1_ICACHE_ASSOC);
          destStruct->coherenceLineSz = 
            sysconf(_SC_LEVEL1_ICACHE_LINESIZE);
          break;
        case DATA_CACHE:
          destStruct->size = 
            sysconf(_SC_LEVEL1_DCACHE_SIZE);
          destStruct->waysOfAssoc = 
            sysconf(_SC_LEVEL1_DCACHE_ASSOC);
          destStruct->coherenceLineSz = 
            sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
          break;
      }
      break;
    case 2:
      destStruct->size = sysconf(_SC_LEVEL2_CACHE_SIZE);
      destStruct->waysOfAssoc = sysconf(_SC_LEVEL2_CACHE_ASSOC);
      destStruct->coherenceLineSz = 
        sysconf(_SC_LEVEL2_CACHE_LINESIZE);
      break;
    case 3:
      destStruct->size = sysconf(_SC_LEVEL3_CACHE_SIZE);
      destStruct->waysOfAssoc = sysconf(_SC_LEVEL3_CACHE_ASSOC);
      destStruct->coherenceLineSz = 
        sysconf(_SC_LEVEL3_CACHE_LINESIZE);
      break;
    case 4:
      destStruct->size = sysconf(_SC_LEVEL4_CACHE_SIZE);
      destStruct->waysOfAssoc = sysconf(_SC_LEVEL4_CACHE_ASSOC);
      destStruct->coherenceLineSz = 
        sysconf(_SC_LEVEL4_CACHE_LINESIZE);
      break;
  }

  destStruct->level = level;
}


}
