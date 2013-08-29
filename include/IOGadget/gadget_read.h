#ifndef _IOGADGET_READ_H

#define _IOGADGET_READ_H

#include <stdio.h>
#include <stdlib.h>	// <- dépendance à retirer.
#include <string.h>
#include <stdbool.h>

#include "IOGadget/types.h"

Particule Gadget_Read_format1(const char *fname, Header *header, int files, bool b_potential, bool b_acceleration, bool b_rate_entropy, bool b_timestep);
Particule Gadget_Read_format2(const char *fname, Header *header, int files, bool b_potential, bool b_acceleration, bool b_rate_entropy, bool b_timestep);

#endif /* end of include guard: GADGET_H */
