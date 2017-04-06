#include <stdio.h>
#include <stdlib.h>
//#include "OPF.h"

int main(int argc, char **argv)
{

	if (argc != 3) 
	{
		fprintf(stderr,"\nusage: txt2csv <txt file name> <binary_descriptor_file_name>\n");
		exit(-1);
	}

	printf("\nPrograma para converter arquivo texto (formato OPF) \nem arquivo texto (formato CSV)");

	FILE *fpIn = NULL,*fpOut = NULL;
	int n, ndata, nclasses, i,j, id,label;
	float aux;
	size_t result;

	fpIn = fopen(argv[1],"r");
	fpOut = fopen(argv[2],"w");

	/*gravando numero de objetos*/
	fscanf(fpIn,"%d",&n); printf("\nnobjects: %d",n);
// 	fwrite(&n,sizeof(int),1,fpOut);

	/*gravando numero de classes*/
	fscanf(fpIn,"%d",&nclasses); 	printf("\nnclasses: %d",nclasses); 
// 	fwrite(&nclasses,sizeof(int),1,fpOut);

	/*gravando tamanho vetor de caracteristicas*/
	fscanf(fpIn,"%d",&ndata); printf("\nndata: %d\n\n",ndata);
// 	fwrite(&ndata,sizeof(int),1,fpOut);
	
	int *countl = (int *) calloc(nclasses+1, sizeof(int));
	/*gravando dados*/
	for(i = 0; i < n; i++)	{
		fscanf(fpIn,"%d",&id);	fprintf(fpOut,"%d,",id);
		fscanf(fpIn,"%d",&label); fprintf(fpOut,"%d,",label);
		countl[label]++;
		for(j = 0; j < ndata; j++){
			fscanf(fpIn,"%f",&aux);
			if (j < ndata-1) 
			    fprintf(fpOut,"%.5f,",aux);
			else
			    fprintf(fpOut,"%.5f\n",aux);
		}
	}
	
	fclose(fpIn);
	fclose(fpOut);

	printf("\nPercentage of each class:\n");
	for (i = 1; i <= nclasses; i++) {
	    float perc_i = ((float)countl[i]/(float)n)*100;
	    printf("\tclass %d: %.2f%% (%d total)\n", i, perc_i, countl[i]);
	}
	


	return 0;
}
