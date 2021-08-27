//===----------------------------------------------------------===//
// Utils module
//
// File purpose: global macro definitions
//===----------------------------------------------------------===//

#ifndef DEFS_H
#define DEFS_H

#define MEASURE_TIME // Uncomment to activate time measuring
#define ALG_TYPE int

// Put here any debugging control macros.
#define INTERFACE_INIT_DEBUG 0
//#define INTERFACE_INIT_DEBUG_PRINT_GREATER_THAN 900000
#define ALG_ERATSIEVE_DEBUG 0

// MPI macros
#define SUPER_EXIT(errcode) {MPI_Finalize(); exit(errcode);}

#endif // #ifndef DEFS_H
////////////////////////////////////////////////////////////////////
