#include "IOGadget/other.h"

void Swap(Particule a, Particule b)
{
	struct _particule_data tmp;

	memcpy(&tmp, a, sizeof(tmp));
	memcpy(a, b, sizeof(tmp));
	memcpy(b, &tmp, sizeof(tmp));
}
