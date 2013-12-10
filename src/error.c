#include "IOGadget/error.h"

static const char *error = NULL;

void SetError(const char *str)
{
	error = str;
}

const char* GetError(void)
{
	return error;
}

