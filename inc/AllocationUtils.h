#ifndef ALLOCATIONUTILS_H
#define ALLOCATIONUTILS_H

#include <TH1D.h>
#include <THStack.h>

// --- Allocation functions
template<class TYPE>
void AllocateArrayX(TYPE **&a, int n1);

template<class TYPE>
void AllocateArrayXY(TYPE ***&a, int n1, int n2);

template<class TYPE>
void AllocateArrayXYZ(TYPE ****&a, int n1, int n2, int n3);

#endif
