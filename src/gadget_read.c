#include "IOGadget/gadget_read.h"

#define SKIP fread(&dummy, sizeof(dummy), 1, fd);
#define SKIP2 fread(&dummy2, sizeof(dummy2), 1, fd);
#define TEST_SS2 if(dummy != dummy2) { SetError("Wrong block!"); return NULL; }

Particule_f Float_Gadget_Read_format1(const char *fname, Header *header, int files, bool b_potential, bool b_acceleration, bool b_rate_entropy, bool b_timestep)
{
	FILE *fd;
	char buf[200];
	int i, k, dummy, dummy2, ntot_withmasses;
	int n, pc, pc_new, pc_sph;
	int NumPart = 0;
	Particule_f P = NULL;

	for(i = 0, pc = 0; i < files; i++, pc = pc_new)
	{
		if(files > 1)
			sprintf(buf, "%s.%d", fname, i);
		else
			sprintf(buf, "%s", fname);

		if(!(fd = fopen(buf, "r")))
		{
			perror("Can't open file:");
			printf("can't open file `%s`\n", buf);
			/*error = "Can't open file.";*/
			SetError("Can't open file.");
			return NULL;
			/*exit(EXIT_FAILURE);*/
		}

		fread(&dummy, sizeof(dummy), 1, fd);
		fread(header, sizeof(Header), 1, fd);
		fread(&dummy, sizeof(dummy), 1, fd);

		if(files == 1)
		{
			for(k = 0, NumPart = 0, ntot_withmasses = 0; k < 6; k++)
				NumPart += header->npart[k];
		}
		else
		{
			for(k = 0, NumPart = 0, ntot_withmasses = 0; k < 6; k++)
				NumPart += header->npartTotal[k];
		}

		for(k = 0, ntot_withmasses = 0; k < 6; k++)
		{
			if(header->mass[k] == 0)
				ntot_withmasses += header->npart[k];
		}

		if(i == 0)
		{
			if( (P = malloc(NumPart * sizeof(struct _particule_data_f))) == NULL )
			{
				perror("Allocate memory failed:");
				SetError("Allocation failed!");
				return NULL;
			}
		}

		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				float tmp[3];
				fread(&tmp, sizeof(float), 3, fd);
				for(int j=0; j<3; j++) P[pc_new].Pos[j] = tmp[j];
				pc_new++;
			}
		}
		SKIP2;
		TEST_SS2;

		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				float tmp[3];
				fread(&tmp, sizeof(float), 3, fd);
				for(int j=0; j<3; j++) P[pc_new].Vit[j] = tmp[j];
				pc_new++;
			}
		}
		SKIP2;
		TEST_SS2;


		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				fread(&P[pc_new].Id, sizeof(int), 1, fd);
				pc_new++;
			}
		}
		SKIP2;
		TEST_SS2;


		if(ntot_withmasses > 0)
			SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				P[pc_new].Type = k;

				if(header->mass[k] == 0)
				{
					float m;
					fread(&m, sizeof(float), 1, fd);
					P[pc_new].m = m;
				}
				else
					P[pc_new].m = header->mass[k];
				pc_new++;
			}
		}
		if(ntot_withmasses > 0)
		{
			SKIP2;
			TEST_SS2;
		}


		if(header->npart[0] > 0)
		{
			SKIP;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				float tmp;
				fread(&tmp, sizeof(float), 1, fd);
				P[pc_sph].U = tmp;
				pc_sph++;
			}
			SKIP2;
			TEST_SS2;

			SKIP;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				float tmp;
				fread(&tmp, sizeof(float), 1, fd);
				P[pc_sph].Rho = tmp;
				pc_sph++;
			}
			SKIP2;
			TEST_SS2;

			if(header->flag_cooling)
			{
				SKIP;
				for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
				{
					float tmp;
					fread(&tmp, sizeof(float), 1, fd);
					P[pc_sph].Ne = tmp;
					pc_sph++;
				}
				SKIP2;
				TEST_SS2;
			}
			else
				for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
				{
					P[pc_sph].Ne = 1.0;
					pc_sph++;
				}
		}

		if( b_potential )
		{
			SKIP;
			float tmp;
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
				{
					fread(&tmp, sizeof(float), 1, fd);
					P[pc_new].Pot = tmp;
				}
			}
			SKIP2;
			TEST_SS2;
		}

		if( b_acceleration )
		{
			SKIP;
			float tmp[3];
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
				{
					fread(&tmp, sizeof(float), 3, fd);
					for(int j=0; j<3; j++) P[pc_new].Acc[j] = tmp[j];
					pc_new++;
				}
			}
			SKIP2;
			TEST_SS2;
		}

		if( b_rate_entropy && header->npart[0] > 0 )
		{
			SKIP;
			float tmp;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				fread(&tmp, sizeof(float), 1, fd);
				P[pc_sph].dAdt = tmp;
				pc_sph++;
			}
			SKIP2;
			TEST_SS2;
		}

		if( b_timestep )
		{
			SKIP;
			float tmp;
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
				{
					fread(&tmp, sizeof(float), 1, fd);
					P[pc_new].ts = tmp;
				}
			}
			SKIP2;
			TEST_SS2;
		}

		fclose(fd);
	}

	return P;
}

Particule_d Double_Gadget_Read_format1(const char *fname, Header *header, int files, bool b_potential, bool b_acceleration, bool b_rate_entropy, bool b_timestep)
{
	FILE *fd;
	char buf[200];
	int i, k, dummy, dummy2, ntot_withmasses;
	int n, pc, pc_new, pc_sph;
	int NumPart = 0;
	Particule_d P = NULL;

	for(i = 0, pc = 0; i < files; i++, pc = pc_new)
	{
		if(files > 1)
			sprintf(buf, "%s.%d", fname, i);
		else
			sprintf(buf, "%s", fname);

		if(!(fd = fopen(buf, "r")))
		{
			perror("Can't open file:");
			printf("can't open file `%s`\n", buf);
			/*error = "Can't open file.";*/
			SetError("Can't open file.");
			return NULL;
			/*exit(EXIT_FAILURE);*/
		}

		fread(&dummy, sizeof(dummy), 1, fd);
		fread(header, sizeof(Header), 1, fd);
		fread(&dummy, sizeof(dummy), 1, fd);

		if(files == 1)
		{
			for(k = 0, NumPart = 0, ntot_withmasses = 0; k < 6; k++)
				NumPart += header->npart[k];
		}
		else
		{
			for(k = 0, NumPart = 0, ntot_withmasses = 0; k < 6; k++)
				NumPart += header->npartTotal[k];
		}

		for(k = 0, ntot_withmasses = 0; k < 6; k++)
		{
			if(header->mass[k] == 0)
				ntot_withmasses += header->npart[k];
		}

		if(i == 0)
		{
			if( (P = malloc(NumPart * sizeof(struct _particule_data_d))) == NULL )
			{
				perror("Allocate memory failed:");
				SetError("Allocation failed!");
				return NULL;
			}
		}

		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				float tmp[3];
				fread(&tmp, sizeof(float), 3, fd);
				for(int j=0; j<3; j++) P[pc_new].Pos[j] = tmp[j];
				pc_new++;
			}
		}
		SKIP2;
		TEST_SS2;

		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				float tmp[3];
				fread(&tmp, sizeof(float), 3, fd);
				for(int j=0; j<3; j++) P[pc_new].Vit[j] = tmp[j];
				pc_new++;
			}
		}
		SKIP2;
		TEST_SS2;


		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				fread(&P[pc_new].Id, sizeof(int), 1, fd);
				pc_new++;
			}
		}
		SKIP2;
		TEST_SS2;


		if(ntot_withmasses > 0)
			SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				P[pc_new].Type = k;

				if(header->mass[k] == 0)
				{
					float m;
					fread(&m, sizeof(float), 1, fd);
					P[pc_new].m = m;
				}
				else
					P[pc_new].m = header->mass[k];
				pc_new++;
			}
		}
		if(ntot_withmasses > 0)
		{
			SKIP2;
			TEST_SS2;
		}


		if(header->npart[0] > 0)
		{
			SKIP;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				float tmp;
				fread(&tmp, sizeof(float), 1, fd);
				P[pc_sph].U = tmp;
				pc_sph++;
			}
			SKIP2;
			TEST_SS2;

			SKIP;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				float tmp;
				fread(&tmp, sizeof(float), 1, fd);
				P[pc_sph].Rho = tmp;
				pc_sph++;
			}
			SKIP2;
			TEST_SS2;

			if(header->flag_cooling)
			{
				SKIP;
				for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
				{
					float tmp;
					fread(&tmp, sizeof(float), 1, fd);
					P[pc_sph].Ne = tmp;
					pc_sph++;
				}
				SKIP2;
				TEST_SS2;
			}
			else
				for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
				{
					P[pc_sph].Ne = 1.0;
					pc_sph++;
				}
		}

		if( b_potential )
		{
			SKIP;
			float tmp;
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
				{
					fread(&tmp, sizeof(float), 1, fd);
					P[pc_new].Pot = tmp;
				}
			}
			SKIP2;
			TEST_SS2;
		}

		if( b_acceleration )
		{
			SKIP;
			float tmp[3];
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
				{
					fread(&tmp, sizeof(float), 3, fd);
					for(int j=0; j<3; j++) P[pc_new].Acc[j] = tmp[j];
					pc_new++;
				}
			}
			SKIP2;
			TEST_SS2;
		}

		if( b_rate_entropy && header->npart[0] > 0 )
		{
			SKIP;
			float tmp;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				fread(&tmp, sizeof(float), 1, fd);
				P[pc_sph].dAdt = tmp;
				pc_sph++;
			}
			SKIP2;
			TEST_SS2;
		}

		if( b_timestep )
		{
			SKIP;
			float tmp;
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
				{
					fread(&tmp, sizeof(float), 1, fd);
					P[pc_new].ts = tmp;
				}
			}
			SKIP2;
			TEST_SS2;
		}

		fclose(fd);
	}

	return P;
}

#define VERIFICATION \
			if( dummy != dummy2 ) \
			{ \
				fprintf(stderr, "%s:%s:%d :: Block size invalid for label %s: %d != %d\n", __FILE__, __func__, __LINE__, label, dummy, dummy2); \
				SetError("Wrong Block size!"); \
				return NULL; /*exit(EXIT_FAILURE); \*/ \
			}
#define GET_LABEL \
			SKIP; \
			fread(label, sizeof(char), 4, fd); \
			fread(&dummy2, sizeof(int), 1, fd); \
			SKIP2; \
			VERIFICATION


Particule_f Float_Gadget_Read_format2(const char *fname, Header *header, int files, bool b_potential, bool b_acceleration, bool b_rate_entropy, bool b_timestep)
{
	FILE *fd;
	char buf[200], label[5] = {0};
	int i, k, dummy, dummy2, ntot_withmasses;
	int n, pc, pc_new, pc_sph;
	int NumPart = 0;
	Particule_f P = NULL;

	for(i = 0, pc = 0; i < files; i++, pc = pc_new)
	{
		if(files > 1)
			sprintf(buf, "%s.%d", fname, i);
		else
			sprintf(buf, "%s", fname);

		if(!(fd = fopen(buf, "r")))
		{
			perror("Can't open file:");
			printf("can't open file `%s`\n", buf);
			/*error = "Can't open file.";*/
			SetError("Can't open file.");
			return NULL;
			/*exit(EXIT_FAILURE);*/
		}

		GET_LABEL;

		SKIP;
		fread(header, sizeof(Header), 1, fd);
		SKIP2;
		VERIFICATION;

		if(files == 1)
		{
			for(k = 0, NumPart = 0, ntot_withmasses = 0; k < 6; k++)
				NumPart += header->npart[k];
		}
		else
		{
			for(k = 0, NumPart = 0, ntot_withmasses = 0; k < 6; k++)
				NumPart += header->npartTotal[k];
		}

		for(k = 0, ntot_withmasses = 0; k < 6; k++)
		{
			if(header->mass[k] == 0)
				ntot_withmasses += header->npart[k];
		}

		if(i == 0)
		{
			if( (P = malloc(NumPart * sizeof(struct _particule_data_f))) == NULL )
			{
				perror("Allocate memory failed:");
				return NULL;
			}
		}

		GET_LABEL;

		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				fread(&P[pc_new].Pos[0], sizeof(float), 3, fd);
				pc_new++;
			}
		}
		SKIP2;
		VERIFICATION;

		GET_LABEL;

		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				fread(&P[pc_new].Vit[0], sizeof(float), 3, fd);
				pc_new++;
			}
		}
		SKIP2;
		VERIFICATION;

		GET_LABEL;

		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				/*fread(&Id[pc_new], sizeof(int), 1, fd);*/
				fread(&P[pc_new].Id, sizeof(int), 1, fd);
				pc_new++;
			}
		}
		SKIP2;
		VERIFICATION;

		if(ntot_withmasses > 0)
		{
			GET_LABEL;
			SKIP;
		}
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				P[pc_new].Type = k;

				if(header->mass[k] == 0)
					fread(&P[pc_new].m, sizeof(float), 1, fd);
				else
					P[pc_new].m = header->mass[k];
				pc_new++;
			}
		}
		if(ntot_withmasses > 0)
		{
			SKIP2;
			VERIFICATION;
		}

		if(header->npart[0] > 0)
		{
			GET_LABEL;
			SKIP;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				fread(&P[pc_sph].U, sizeof(float), 1, fd);
				pc_sph++;
			}
			SKIP2;
			VERIFICATION;

			GET_LABEL;
			SKIP;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				fread(&P[pc_sph].Rho, sizeof(float), 1, fd);
				pc_sph++;
			}
			SKIP2;
			VERIFICATION;

			if(header->flag_cooling)
			{
				GET_LABEL;
				SKIP;
				for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
				{
					fread(&P[pc_sph].Ne, sizeof(float), 1, fd);
					pc_sph++;
				}
				SKIP2;
				VERIFICATION;
			}
			else
				for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
				{
					P[pc_sph].Ne = 1.0;
					pc_sph++;
				}
		}

		if( b_potential )
		{
			GET_LABEL;
			SKIP;
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
					fread(&P[pc_new].Pot, sizeof(float), 1, fd);
			}
			SKIP2;
			VERIFICATION;
		}

		if( b_acceleration )
		{
			GET_LABEL;
			SKIP;
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
					fread(&P[pc_new].Acc[0], sizeof(float), 3, fd);
			}
			SKIP2;
			VERIFICATION;
		}

		if( b_rate_entropy && header->npart[0] > 0 )
		{
			GET_LABEL;
			SKIP;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				fread(&P[pc_sph].dAdt, sizeof(float), 1, fd);
				pc_sph++;
			}
			SKIP2;
			VERIFICATION;
		}

		if( b_timestep )
		{
			GET_LABEL;
			SKIP;
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
				{
					fread(&P[pc_new].ts, sizeof(float), 1, fd);
				}
			}
			SKIP2;
			VERIFICATION;
		}

		fclose(fd);
	}

	return P;
}

Particule_d Double_Gadget_Read_format2(const char *fname, Header *header, int files, bool b_potential, bool b_acceleration, bool b_rate_entropy, bool b_timestep)
{
	FILE *fd;
	char buf[200], label[5] = {0};
	int i, k, dummy, dummy2, ntot_withmasses;
	int n, pc, pc_new, pc_sph;
	int NumPart = 0;
	Particule_d P = NULL;

	for(i = 0, pc = 0; i < files; i++, pc = pc_new)
	{
		if(files > 1)
			sprintf(buf, "%s.%d", fname, i);
		else
			sprintf(buf, "%s", fname);

		if(!(fd = fopen(buf, "r")))
		{
			perror("Can't open file:");
			printf("can't open file `%s`\n", buf);
			/*error = "Can't open file.";*/
			SetError("Can't open file.");
			return NULL;
			/*exit(EXIT_FAILURE);*/
		}

		GET_LABEL;

		SKIP;
		fread(header, sizeof(Header), 1, fd);
		SKIP2;
		VERIFICATION;

		if(files == 1)
		{
			for(k = 0, NumPart = 0, ntot_withmasses = 0; k < 6; k++)
				NumPart += header->npart[k];
		}
		else
		{
			for(k = 0, NumPart = 0, ntot_withmasses = 0; k < 6; k++)
				NumPart += header->npartTotal[k];
		}

		for(k = 0, ntot_withmasses = 0; k < 6; k++)
		{
			if(header->mass[k] == 0)
				ntot_withmasses += header->npart[k];
		}

		if(i == 0)
		{
			if( (P = malloc(NumPart * sizeof(struct _particule_data_d))) == NULL )
			{
				perror("Allocate memory failed:");
				return NULL;
			}
		}

		GET_LABEL;

		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				fread(&P[pc_new].Pos[0], sizeof(float), 3, fd);
				pc_new++;
			}
		}
		SKIP2;
		VERIFICATION;

		GET_LABEL;

		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				fread(&P[pc_new].Vit[0], sizeof(float), 3, fd);
				pc_new++;
			}
		}
		SKIP2;
		VERIFICATION;

		GET_LABEL;

		SKIP;
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				/*fread(&Id[pc_new], sizeof(int), 1, fd);*/
				fread(&P[pc_new].Id, sizeof(int), 1, fd);
				pc_new++;
			}
		}
		SKIP2;
		VERIFICATION;

		if(ntot_withmasses > 0)
		{
			GET_LABEL;
			SKIP;
		}
		for(k = 0, pc_new = pc; k < 6; k++)
		{
			for(n = 0; n < header->npart[k]; n++)
			{
				P[pc_new].Type = k;

				if(header->mass[k] == 0)
					fread(&P[pc_new].m, sizeof(float), 1, fd);
				else
					P[pc_new].m = header->mass[k];
				pc_new++;
			}
		}
		if(ntot_withmasses > 0)
		{
			SKIP2;
			VERIFICATION;
		}

		if(header->npart[0] > 0)
		{
			GET_LABEL;
			SKIP;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				fread(&P[pc_sph].U, sizeof(float), 1, fd);
				pc_sph++;
			}
			SKIP2;
			VERIFICATION;

			GET_LABEL;
			SKIP;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				fread(&P[pc_sph].Rho, sizeof(float), 1, fd);
				pc_sph++;
			}
			SKIP2;
			VERIFICATION;

			if(header->flag_cooling)
			{
				GET_LABEL;
				SKIP;
				for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
				{
					fread(&P[pc_sph].Ne, sizeof(float), 1, fd);
					pc_sph++;
				}
				SKIP2;
				VERIFICATION;
			}
			else
				for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
				{
					P[pc_sph].Ne = 1.0;
					pc_sph++;
				}
		}

		if( b_potential )
		{
			GET_LABEL;
			SKIP;
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
					fread(&P[pc_new].Pot, sizeof(float), 1, fd);
			}
			SKIP2;
			VERIFICATION;
		}

		if( b_acceleration )
		{
			GET_LABEL;
			SKIP;
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
					fread(&P[pc_new].Acc[0], sizeof(float), 3, fd);
			}
			SKIP2;
			VERIFICATION;
		}

		if( b_rate_entropy && header->npart[0] > 0 )
		{
			GET_LABEL;
			SKIP;
			for(n = 0, pc_sph = pc; n < header->npart[0]; n++)
			{
				fread(&P[pc_sph].dAdt, sizeof(float), 1, fd);
				pc_sph++;
			}
			SKIP2;
			VERIFICATION;
		}

		if( b_timestep )
		{
			GET_LABEL;
			SKIP;
			for(k = 0, pc_new = pc; k < 6; k++)
			{
				for(n = 0; n < header->npart[k]; n++)
				{
					fread(&P[pc_new].ts, sizeof(float), 1, fd);
				}
			}
			SKIP2;
			VERIFICATION;
		}

		fclose(fd);
	}

	return P;
}

#ifdef USE_HDF5
void read_header_attributes_in_hdf5(char *fname)
{
	hid_t hdf5_file, hdf5_headergrp, hdf5_attribute;


	hdf5_file = H5Fopen(fname, H5F_ACC_RDONLY, H5P_DEFAULT);
	hdf5_headergrp = H5Gopen(hdf5_file, "/Header");


	hdf5_attribute = H5Aopen_name(hdf5_headergrp, "NumPart_ThisFile");
	H5Aread(hdf5_attribute, H5T_NATIVE_INT, header.npart);
	H5Aclose(hdf5_attribute);

	hdf5_attribute = H5Aopen_name(hdf5_headergrp, "NumPart_Total");
	H5Aread(hdf5_attribute, H5T_NATIVE_UINT, header.npartTotal);
	H5Aclose(hdf5_attribute);

	hdf5_attribute = H5Aopen_name(hdf5_headergrp, "NumPart_Total_HighWord");
	H5Aread(hdf5_attribute, H5T_NATIVE_UINT, header.npartTotalHighWord);
	H5Aclose(hdf5_attribute);

	hdf5_attribute = H5Aopen_name(hdf5_headergrp, "MassTable");
	H5Aread(hdf5_attribute, H5T_NATIVE_DOUBLE, header.mass);
	H5Aclose(hdf5_attribute);

	hdf5_attribute = H5Aopen_name(hdf5_headergrp, "Time");
	H5Aread(hdf5_attribute, H5T_NATIVE_DOUBLE, &header.time);
	H5Aclose(hdf5_attribute);

	hdf5_attribute = H5Aopen_name(hdf5_headergrp, "NumFilesPerSnapshot");
	H5Aread(hdf5_attribute, H5T_NATIVE_INT, &header.num_files);
	H5Aclose(hdf5_attribute);

	hdf5_attribute = H5Aopen_name(hdf5_headergrp, "Flag_Entropy_ICs");
	H5Aread(hdf5_attribute, H5T_NATIVE_INT, &header.flag_entropy_instead_u);
	H5Aclose(hdf5_attribute);

	H5Gclose(hdf5_headergrp);
	H5Fclose(hdf5_file);
}
#endif
