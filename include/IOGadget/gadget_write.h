#ifndef _IOGADGET_WRITE_H
#define _IOGADGET_WRITE_H

#include <stdio.h>
#include <stdlib.h>	// <- dépendance à retirer.
#include <string.h>
#include <stdbool.h>

#include "IOGadget/types.h"

bool Float_Gadget_Write_format1(const char *name, const Header header, const Particule part);
bool Float_Gadget_Write_format2(const char *name, const Header header, const Particule part);

#endif /* end of include guard */
