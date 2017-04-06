#include "OPF.h"

float opf_ROCAccuracy(Subgraph *sg, float **scoreMatrix, int classind){
	float Acc = 0.0f, **error_matrix = NULL, error = 0.0f;
	int i, *nclass = NULL, nlabels=0;

	error_matrix = (float **)calloc(sg->nlabels+1, sizeof(float *));
	for(i=0; i<= sg->nlabels; i++)
	  error_matrix[i] = (float *)calloc(2, sizeof(float));

	nclass = AllocIntArray(sg->nlabels+1);

	for (i = 0; i < sg->nnodes; i++){
	  nclass[sg->node[i].truelabel]++;
	}

	for (i = 0; i < sg->nnodes; i++){
	  if(sg->node[i].truelabel != sg->node[i].label){
	    error_matrix[sg->node[i].truelabel][1]++;
	    error_matrix[sg->node[i].label][0]++;
	  }
	}

	for(i=1; i <= sg->nlabels; i++){
	  if (nclass[i]!=0){
	    error_matrix[i][1] /= (float)nclass[i];
	    error_matrix[i][0] /= (float)(sg->nnodes - nclass[i]);
	    nlabels++;
	  }
	}

	for(i=1; i <= sg->nlabels; i++){
	  if (nclass[i]!=0)
	    error += (error_matrix[i][0]+error_matrix[i][1]);
	}

	Acc = 1.0-(error/(2.0*nlabels));

	for(i=0; i <= sg->nlabels; i++)
	  free(error_matrix[i]);
	free(error_matrix);
	free(nclass);

	return(Acc);
}


int main(int argc, char **argv){
	fflush(stdout);
	fprintf(stdout, "\nProgram that computes OPF accuracy of a given set\n");
	fprintf(stdout, "\nIf you have any problem, please contact: ");
	fprintf(stdout, "\n- alexandre.falcao@gmail.com");
	fprintf(stdout, "\n- papa.joaopaulo@gmail.com\n");
	fprintf(stdout, "\nLibOPF version 2.0 (2009)\n");
	fprintf(stdout, "\n"); fflush(stdout);

	if(argc < 3){
		fprintf(stderr, "\nusage opf_roc <P1> <P2> <P3>");
		fprintf(stderr, "\nP1: test data set in the OPF file format");
		fprintf(stderr, "\nP2: scores for the dataset objects");
		fprintf(stderr, "\nP3: class index: default=1");
		exit(-1);
	}
	int classind;
	
	if(argc==3) {
	  classind = 1;
	}

	int i, p, nlabels, nnodes;
	float Acc;
	size_t result;
	FILE *f = NULL;
	char fileName[256];
	float **scoreMatrix;
	

	fprintf(stdout, "\nReading data file ..."); fflush(stdout);
	Subgraph *g = ReadSubgraph(argv[1]);
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nReading scores file ..."); fflush(stdout);
	sprintf(fileName,"%s",argv[2]);
	f = fopen(fileName,"r");
	if(!f){
		fprintf(stderr,"\nunable to open file %s", fileName);
		exit(-1);
	}
	nlabels = g->nlabels;
	nnodes = g->nnodes;
	scoreMatrix = (float **) malloc(nnodes*sizeof(float *));
	for (i=0; i<nnodes; i++) {
	    scoreMatrix[i] = (float *) malloc(nlabels*sizeof(float));
	    for (p=0; p<nlabels; p++)
		result = fscanf(f,"%f",&scoreMatrix[i][p]);
	}
	
	fclose(f);
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nComputing accuracy ..."); fflush(stdout);
	Acc = opf_ROCAccuracy(g,scoreMatrix,classind);
	fprintf(stdout, "\nAccuracy: %.2f%%", Acc*100); fflush(stdout);

	fprintf(stdout, "\nWriting accuracy in output file ..."); fflush(stdout);
	sprintf(fileName,"%s.acc",argv[1]);
	f = fopen(fileName,"a");
	result = fprintf(f,"%f\n",Acc*100);
	fclose(f);
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nDeallocating memory ..."); fflush(stdout);
	DestroySubgraph(&g);
	fprintf(stdout, " OK\n");

	return 0;
}
