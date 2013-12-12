#include "IOGadget/other.h"

void Double_Swap(Particule_d a, Particule_d b)
{
	struct _particule_data_d tmp;

	memcpy(&tmp, a, sizeof(tmp));
	memcpy(a, b, sizeof(tmp));
	memcpy(b, &tmp, sizeof(tmp));
}

void Float_Swap(Particule_f a, Particule_f b)
{
	struct _particule_data_f tmp;

	memcpy(&tmp, a, sizeof(tmp));
	memcpy(a, b, sizeof(tmp));
	memcpy(b, &tmp, sizeof(tmp));
}
