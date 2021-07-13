//===----------------------------------------------------------===//
// Utils module
//
// File purpose: declaration of hardware info related objects. By
// hardware info we mean functions that encapsulate the fetching of
// hardware-related information.
//===----------------------------------------------------------===//

#ifndef HWINFO_H
#define HWINFO_H

#include <tuple>

namespace Utils {

static constexpr unsigned kmaxCacheTypeLen = 0x10;
// Values initialized as -1 to signal that they have not been
// defined later.
struct cacheInfo {
  // These are given by sysconf
  int coherenceLineSz = -1;
  int size = -1;
  int waysOfAssoc = -1;

  int level = -1;

#if defined(__linux__)
  int id = -1;
  int numberOfSets = -1;
  int sharedCpuMap = -1;
  std::tuple<int, int> sharedCpuList = std::make_tuple(-1, -1);
  char type[kmaxCacheTypeLen];
  int physicalLinePartition = -1;
#endif
};

#define LEVEL1 1
#define LEVEL2 2
#define LEVEL3 3
#define LEVEL4 4

#define INSTRUCTION_CACHE 0
#define DATA_CACHE 1

class hwInfo {
public:
  // Simply builds the object according to system information.
  static void fetchCacheInfo(cacheInfo* const destStruct,
                             const unsigned level,
                             const unsigned iOrC);
};

}

#endif
