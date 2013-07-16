#include "gadget/gadget_write.h"

#define SKIP  {fwrite(&blksize,sizeof(int),1,fd);}

bool Gadget_Write_format1(const char *name, const Header header, const Particule part)
{
	FILE *fd = NULL;
	int blksize, NbPart = 0, ntot_withmasses = 0, pc_new = 0;
	float to_write;

	//fprintf(stderr, "File name : %s\n", name);

	if(!(fd = fopen(name, "w")))
	{
		perror("Can't open file for writing snapshot: ");
		return false;
	}

	//printf("\033[36mHeader du fichier Gadget (format 1) :\033[00m\n");
	//printf("\033[34m\tNombre de fichier par snapshot : %d\n", header.num_files);
	//printf("\tMasse et nombre d'éléments des catégories d'objet :\n");
	//for(int i = 0; i < 6; i++)
		//printf("\t\t%s : Masse %g, et %d élément%c (total : %d)\n", (i == 0)?"Gaz":( (i == 1)?"Halo":( (i == 2)?"Disk":( (i==3)?"Bulge":( (i==4)?"Stars":"Bndry" )))), header.mass[i], header.npart[i], (header.npart[i] > 1)?'s':' ', header.npartTotal[i]);
	//puts("\033[00m");
	//printf("\033[31m\tTaille de la boîte : %g\033[00m\n", header.BoxSize);

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
		for(int j=0; j<3; j++)
		{
			to_write = (float)part[i].Pos[j];
			fwrite(&to_write, sizeof(float), 1, fd);
		}
	SKIP;

	SKIP;
	for(int i=0; i<NbPart; i++)
		for(int j=0; j<3; j++)
		{
			to_write = (float)part[i].Vit[j];
			fwrite(&to_write, sizeof(float), 1, fd);
		}
	SKIP;

	blksize = NbPart * sizeof(unsigned int);
	SKIP;
	for(int i=0; i<NbPart; i++)
		fwrite(&part[i].Id, sizeof(unsigned int), 1, fd);
	SKIP;

	blksize = ntot_withmasses*sizeof(float);
	if( ntot_withmasses > 0 )
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
	if( ntot_withmasses > 0 )
		SKIP;

	return true;
}

#define LABEL(M_label, M_nextblock) fwrite(&label_size, sizeof(int), 1, fd); \
	fwrite(&(M_label), sizeof(char), 4, fd); \
	fwrite(&(M_nextblock), sizeof(int), 1, fd); \
	fwrite(&label_size, sizeof(int), 1, fd);

bool Gadget_Write_format2(const char *name, const Header header, const Particule part)
{
	FILE *fd            = NULL;
	int blksize         = 0,
	    NbPart          = 0,
	    ntot_withmasses = 0,
	    pc_new          = 0,
	    n               = 0,
	    label_size      = 4*sizeof(char);
	float to_write;
	const char *label;

	if(!(fd = fopen(name, "w")))
	{
		perror("Can't open file for writing snapshot: ");
		return false;
	}

	blksize = sizeof(header);
	label = "HEAD";
	LABEL(label, blksize);
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
	LABEL(label, blksize);
	SKIP;
	for(int i=0; i<NbPart; i++)
		for(int j=0; j<3; j++)
		{
			to_write = (float)part[i].Pos[j];
			fwrite(&to_write, sizeof(float), 1, fd);
		}
	SKIP;

	label = "VEL ";
	LABEL(label, blksize);
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
	LABEL(label, blksize);
	SKIP;
	for(int i=0; i<NbPart; i++)
		fwrite(&part[i].Id, sizeof(unsigned int), 1, fd);
	SKIP;

	blksize = ntot_withmasses*sizeof(float);
	if( ntot_withmasses > 0 )
	{
		label = "MASS";
		LABEL(label, blksize);
		SKIP;
	}
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
	if( ntot_withmasses > 0 )
		SKIP;

	if(header.npart[0] > 0)
	{
		label = "U   ";
		LABEL(label, blksize);
		SKIP;
		for(n = 0; n < header.npart[0]; n++)
		{
			fread(&part[n].U, sizeof(float), 1, fd);
		}
		SKIP;

		label = "RHO ";
		LABEL(label, blksize);
		SKIP;
		for(n = 0; n < header.npart[0]; n++)
		{
			fread(&part[n].Rho, sizeof(float), 1, fd);
		}
		SKIP;

		if(header.flag_cooling)
		{
			label = "HSML";
			LABEL(label, blksize);
			SKIP;
			for(n = 0; n < header.npart[0]; n++)
			{
				fread(&part[n].Ne, sizeof(float), 1, fd);
			}
			SKIP;
		}
	}

	return true;
}
