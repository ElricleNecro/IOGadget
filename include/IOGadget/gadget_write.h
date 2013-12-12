#ifndef _IOGADGET_WRITE_H
#define _IOGADGET_WRITE_H

#include <stdio.h>
#include <stdlib.h>	// <- dépendance à retirer.
#include <string.h>
#include <stdbool.h>

#include "IOGadget/types.h"

bool Float_Gadget_Write_format1(const char *name, const Header header, const Particule_f part);
bool Float_Gadget_Write_format2(const char *name, const Header header, const Particule_f part);

bool Double_Gadget_Write_format1(const char *name, const Header header, const Particule_d part);
bool Double_Gadget_Write_format2(const char *name, const Header header, const Particule_d part);

#endif /* end of include guard */
