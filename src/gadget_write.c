#include "IOGadget/gadget_write.h"

#define SKIP  {fwrite(&blksize,sizeof(int),1,fd);}

bool Float_Gadget_Write_format1(const char *name, const Header header, const Particule_f part)
{
	FILE *fd = NULL;
	int blksize, NbPart = 0, ntot_withmasses = 0, pc_new = 0;

	if(!(fd = fopen(name, "w")))
	{
		perror("Can't open file for writing snapshot: ");
		return false;
	}

	blksize = sizeof(header);
	SKIP;
	fwrite(&header, sizeof(header), 1, fd);
	SKIP;

	blksize = 0;
	for (int n = 0; n < 6; n++)
	{
		NbPart          += header.npart[n];
		blksize         += header.npart[n];
		if( header.mass[n] == 0 )
			ntot_withmasses += header.npart[n];
	}
	blksize *= 3 * sizeof(float);

	SKIP;
	for(int i=0; i<NbPart; i++)
	{
		float tmp[3];
		for(int j=0; j<3; j++)
			tmp[j] = part[i].Pos[j];

		for(int j=0; j<3; j++)
		{
			fwrite(&tmp[j], sizeof(float), 1, fd);
		}
	}
	SKIP;

	SKIP;
	for(int i=0; i<NbPart; i++)
	{
		float tmp[3];
		for(int j=0; j<3; j++)
			tmp[j] = part[i].Vit[j];

		for(int j=0; j<3; j++)
		{
			fwrite(&tmp[j], sizeof(float), 1, fd);
		}
	}
	SKIP;

	blksize = NbPart * sizeof(unsigned int);
	SKIP;
	for(int i=0; i<NbPart; i++)
		fwrite(&part[i].Id, sizeof(unsigned int), 1, fd);
	SKIP;

	blksize = ntot_withmasses*sizeof(float);
	if( ntot_withmasses > 0 )
	{
		SKIP;
		double tmp;
		for(int i = 0; i < 6; i++)
		{
			if( header.mass[i] == 0. && header.npart[i] != 0 )
			{
				for(int k=0; k<header.npart[i]; k++)
				{
					tmp = part[pc_new].m;
					fwrite(&tmp, sizeof(float), 1, fd);
					pc_new++;
				}
			}
		}
		SKIP;
	}

	fclose(fd);

	return true;
}

bool Double_Gadget_Write_format1(const char *name, const Header header, const Particule_d part)
{
	FILE *fd = NULL;
	int blksize, NbPart = 0, ntot_withmasses = 0, pc_new = 0;

	if(!(fd = fopen(name, "w")))
	{
		perror("Can't open file for writing snapshot: ");
		return false;
	}

	blksize = sizeof(header);
	SKIP;
	fwrite(&header, sizeof(header), 1, fd);
	SKIP;

	blksize = 0;
	for (int n = 0; n < 6; n++)
	{
		NbPart          += header.npart[n];
		blksize         += header.npart[n];
		if( header.mass[n] == 0 )
			ntot_withmasses += header.npart[n];
	}
	blksize *= 3 * sizeof(float);

	SKIP;
	for(int i=0; i<NbPart; i++)
	{
		float tmp[3];
		for(int j=0; j<3; j++)
			tmp[j] = part[i].Pos[j];

		for(int j=0; j<3; j++)
		{
			fwrite(&tmp[j], sizeof(float), 1, fd);
		}
	}
	SKIP;

	SKIP;
	for(int i=0; i<NbPart; i++)
	{
		float tmp[3];
		for(int j=0; j<3; j++)
			tmp[j] = part[i].Vit[j];

		for(int j=0; j<3; j++)
		{
			fwrite(&tmp[j], sizeof(float), 1, fd);
		}
	}
	SKIP;

	blksize = NbPart * sizeof(unsigned int);
	SKIP;
	for(int i=0; i<NbPart; i++)
		fwrite(&part[i].Id, sizeof(unsigned int), 1, fd);
	SKIP;

	blksize = ntot_withmasses*sizeof(float);
	if( ntot_withmasses > 0 )
	{
		SKIP;
		double tmp;
		for(int i = 0; i < 6; i++)
		{
			if( header.mass[i] == 0. && header.npart[i] != 0 )
			{
				for(int k=0; k<header.npart[i]; k++)
				{
					tmp = part[pc_new].m;
					fwrite(&tmp, sizeof(float), 1, fd);
					pc_new++;
				}
			}
		}
		SKIP;
	}

	fclose(fd);

	return true;
}

#define LABEL(M_label, M_nextblock) fwrite(&label_size, sizeof(int), 1, fd); \
	fwrite(M_label, sizeof(char), 4, fd); \
	fwrite(&(M_nextblock), sizeof(int), 1, fd); \
	fwrite(&label_size, sizeof(int), 1, fd);

bool Float_Gadget_Write_format2(const char *name, const Header header, const Particule_f part)
{
	FILE *fd            = NULL;
	int blksize         = 0,
	    NbPart          = 0,
	    ntot_withmasses = 0,
	    pc_new          = 0,
	    n               = 0,
	    label_size      = 4*sizeof(char) + sizeof(int),
	    nextblock       = 0;
	float to_write;
	const char *label;

	if(!(fd = fopen(name, "w")))
	{
		perror("Can't open file for writing snapshot: ");
		return false;
	}

	blksize = sizeof(header);
	label = "HEAD";
	printf("%s\n", label);
	nextblock = sizeof(header) + 2*sizeof(int);
	LABEL(label, nextblock);
	SKIP;
	fwrite(&header, sizeof(header), 1, fd);
	SKIP;

	blksize = 0;
	for (int i = 0; i < 6; i++)
	{
		NbPart          += header.npart[i];
		blksize         += header.npart[i];
		if( header.mass[i] == 0 )
			ntot_withmasses += header.npart[i];
	}
	blksize *= 3 * sizeof(float);

	label = "POS ";
	nextblock = blksize + 2*sizeof(int);
	LABEL(label, nextblock);
	SKIP;
	for(int i=0; i<NbPart; i++)
		for(int j=0; j<3; j++)
		{
			to_write = (float)part[i].Pos[j];
			fwrite(&to_write, sizeof(float), 1, fd);
		}
	SKIP;

	label = "VEL ";
	nextblock = blksize + 2*sizeof(int);
	LABEL(label, nextblock);
	SKIP;
	for(int i=0; i<NbPart; i++)
		for(int j=0; j<3; j++)
		{
			to_write = (float)part[i].Vit[j];
			fwrite(&to_write, sizeof(float), 1, fd);
		}
	SKIP;

	blksize = NbPart * sizeof(unsigned int);
	label = "ID  ";
	nextblock = blksize + 2*sizeof(int);
	LABEL(label, nextblock);
	SKIP;
	for(int i=0; i<NbPart; i++)
		fwrite(&part[i].Id, sizeof(unsigned int), 1, fd);
	SKIP;

	blksize = ntot_withmasses*sizeof(float);
	if( ntot_withmasses > 0 )
	{
		label = "MASS";
		nextblock = blksize + 2*sizeof(int);
		LABEL(label, nextblock);
		SKIP;
		for(int i = 0; i < 6; i++)
		{
			if( header.mass[i] == 0. && header.npart[i] != 0 )
			{
				for(int k=0; k<header.npart[i]; k++)
				{
					to_write = (float)part[pc_new].m;
					fwrite(&to_write, sizeof(float), 1, fd);
					pc_new++;
				}
			}
		}
		SKIP;
	}

	if(header.npart[0] > 0)
	{
		label = "U   ";
		nextblock = blksize + 2*sizeof(int);
		LABEL(label, nextblock);
		SKIP;
		for(n = 0; n < header.npart[0]; n++)
		{
			fread(&part[n].U, sizeof(float), 1, fd);
		}
		SKIP;

		label = "RHO ";
		nextblock = blksize + 2*sizeof(int);
		LABEL(label, nextblock);
		SKIP;
		for(n = 0; n < header.npart[0]; n++)
		{
			fread(&part[n].Rho, sizeof(float), 1, fd);
		}
		SKIP;

		if(header.flag_cooling)
		{
			label = "HSML";
			nextblock = blksize + 2*sizeof(int);
			LABEL(label, nextblock);
			SKIP;
			for(n = 0; n < header.npart[0]; n++)
			{
				fread(&part[n].Ne, sizeof(float), 1, fd);
			}
			SKIP;
		}
	}

	fclose(fd);

	return true;
}

bool Double_Gadget_Write_format2(const char *name, const Header header, const Particule_d part)
{
	FILE *fd            = NULL;
	int blksize         = 0,
	    NbPart          = 0,
	    ntot_withmasses = 0,
	    pc_new          = 0,
	    n               = 0,
	    label_size      = 4*sizeof(char) + sizeof(int),
	    nextblock       = 0;
	float to_write;
	const char *label;

	if(!(fd = fopen(name, "w")))
	{
		perror("Can't open file for writing snapshot: ");
		return false;
	}

	blksize = sizeof(header);
	label = "HEAD";
	printf("%s\n", label);
	nextblock = sizeof(header) + 2*sizeof(int);
	LABEL(label, nextblock);
	SKIP;
	fwrite(&header, sizeof(header), 1, fd);
	SKIP;

	blksize = 0;
	for (int i = 0; i < 6; i++)
	{
		NbPart          += header.npart[i];
		blksize         += header.npart[i];
		if( header.mass[i] == 0 )
			ntot_withmasses += header.npart[i];
	}
	blksize *= 3 * sizeof(float);

	label = "POS ";
	nextblock = blksize + 2*sizeof(int);
	LABEL(label, nextblock);
	SKIP;
	for(int i=0; i<NbPart; i++)
		for(int j=0; j<3; j++)
		{
			to_write = (float)part[i].Pos[j];
			fwrite(&to_write, sizeof(float), 1, fd);
		}
	SKIP;

	label = "VEL ";
	nextblock = blksize + 2*sizeof(int);
	LABEL(label, nextblock);
	SKIP;
	for(int i=0; i<NbPart; i++)
		for(int j=0; j<3; j++)
		{
			to_write = (float)part[i].Vit[j];
			fwrite(&to_write, sizeof(float), 1, fd);
		}
	SKIP;

	blksize = NbPart * sizeof(unsigned int);
	label = "ID  ";
	nextblock = blksize + 2*sizeof(int);
	LABEL(label, nextblock);
	SKIP;
	for(int i=0; i<NbPart; i++)
		fwrite(&part[i].Id, sizeof(unsigned int), 1, fd);
	SKIP;

	blksize = ntot_withmasses*sizeof(float);
	if( ntot_withmasses > 0 )
	{
		label = "MASS";
		nextblock = blksize + 2*sizeof(int);
		LABEL(label, nextblock);
		SKIP;
		for(int i = 0; i < 6; i++)
		{
			if( header.mass[i] == 0. && header.npart[i] != 0 )
			{
				for(int k=0; k<header.npart[i]; k++)
				{
					to_write = (float)part[pc_new].m;
					fwrite(&to_write, sizeof(float), 1, fd);
					pc_new++;
				}
			}
		}
		SKIP;
	}

	if(header.npart[0] > 0)
	{
		label = "U   ";
		nextblock = blksize + 2*sizeof(int);
		LABEL(label, nextblock);
		SKIP;
		for(n = 0; n < header.npart[0]; n++)
		{
			fread(&part[n].U, sizeof(float), 1, fd);
		}
		SKIP;

		label = "RHO ";
		nextblock = blksize + 2*sizeof(int);
		LABEL(label, nextblock);
		SKIP;
		for(n = 0; n < header.npart[0]; n++)
		{
			fread(&part[n].Rho, sizeof(float), 1, fd);
		}
		SKIP;

		if(header.flag_cooling)
		{
			label = "HSML";
			nextblock = blksize + 2*sizeof(int);
			LABEL(label, nextblock);
			SKIP;
			for(n = 0; n < header.npart[0]; n++)
			{
				fread(&part[n].Ne, sizeof(float), 1, fd);
			}
			SKIP;
		}
	}

	fclose(fd);

	return true;
}
