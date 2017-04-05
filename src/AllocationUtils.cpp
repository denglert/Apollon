#include <AllocationUtils.h>

template<class TYPE>
void AllocateArrayX(TYPE **&a, int n1)
{
	a = new TYPE*[n1];
}

template<class TYPE>
void AllocateArrayXY(TYPE ***&a, int n1, int n2)
{
	a = new TYPE**[n1];

	for (int i1 = 0; i1 < n1; i1++)
	{
		a[i1] = new TYPE*[n2];
	}
}

template<class TYPE>
void AllocateArrayXYZ(TYPE ****&a, int n1, int n2, int n3)
{
	a = new TYPE***[n1];

	for (int i1 = 0; i1 < n1; i1++)
	{
		a[i1] = new TYPE**[n2];

		for (int i2 = 0; i2 < n2; i2++)
		{
			a[i1][i2] = new TYPE*[n3];
		}

	}
}

// -- Allocate function template specializations
template void AllocateArrayX  <TH1D>(TH1D   **&a, int n1                );
template void AllocateArrayXY <TH1D>(TH1D  ***&a, int n1, int n2        );
template void AllocateArrayXYZ<TH1D>(TH1D ****&a, int n1, int n2, int n3);
template void AllocateArrayX  <THStack>(THStack   **&a, int n1                );
template void AllocateArrayXY <THStack>(THStack  ***&a, int n1, int n2        );
template void AllocateArrayXYZ<THStack>(THStack ****&a, int n1, int n2, int n3);
