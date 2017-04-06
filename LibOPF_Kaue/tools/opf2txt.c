#include <stdio.h>
#include <stdlib.h>
//#include "OPF.h"

int main(int argc, char **argv)
{
	int Nf;
	if (argc < 3) 
	{
		fprintf(stderr,"\nusage: ascii2bin <txt file name> <binary_descriptor_file_name>\n");
		exit(-1);
	}

	printf("\nPrograma para converter arquivo binario (formato OPF) \nem arquivo texto");

	FILE *fpIn = NULL,*fpOut = NULL;
	int n, ndata, nclasses, i,j, id,label;
	float aux;
	size_t result;

	fpIn = fopen(argv[1],"rb");
	fpOut = fopen(argv[2],"w");

	/*gravando numero de objetos*/
	fread(&n,sizeof(int),1,fpIn);
	fprintf(fpOut,"%d\t",n); printf("\nnobjects: %d", n);

	/*gravando numero de classes*/
	fread(&nclasses,sizeof(int),1,fpIn);
	fprintf(fpOut,"%d\t",nclasses); printf("\nnclasses: %d", nclasses); 

	/*gravando tamanho vetor de caracteristicas*/
	fread(&ndata,sizeof(int),1,fpIn);
	
	if (argc == 4) Nf = atoi(argv[3]);
	else Nf = ndata;

	fprintf(fpOut,"%d\n",Nf); printf("\nndata: %d\n\n", Nf);
	
	
	/*gravando dados*/
	for(i = 0; i < n; i++)	{
		fread(&id,sizeof(int),1,fpIn);
		fprintf(fpOut,"%d\t",i);	
		
		fread(&label,sizeof(int),1,fpIn);
		fprintf(fpOut,"%d\t",label);
		
		for(j = 0; j < ndata; j++) {
		  fread(&aux,sizeof(float),1,fpIn);
		  if (j < Nf)  fprintf(fpOut,"%.6f\t",aux);
		}
		fprintf(fpOut,"\n");
	}


	fclose(fpIn);
	fclose(fpOut);

	return 0;
}
