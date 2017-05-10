#include "OPF.h"

void getPrototypes(Subgraph *g) {
	FILE *fp = NULL;
	int   i, j;
	size_t result;

	fp = fopen("prototypes.txt", "w");
	result = fwrite(&g->nnodes, sizeof(int), 1, fp);
	result = fwrite(&g->nlabels, sizeof(int), 1, fp);
	result = fwrite(&g->nfeats, sizeof(int), 1, fp);

	/*writing df*/
	result = fwrite(&g->df, sizeof(float), 1, fp);

	// for supervised opf based on pdf

	result = fwrite(&g->K, sizeof(float), 1, fp);
	result = fwrite(&g->mindens, sizeof(float), 1, fp);
	result = fwrite(&g->maxdens, sizeof(float), 1, fp);

	/*writing position(id), label, pred, pathval and features*/
	for (i = 0; i < g->nnodes; i++){
		if (g->node[i].status == opf_PROTOTYPE) {
			result = fwrite(&g->node[i].position, sizeof(int), 1, fp);
			result = fwrite(&g->node[i].truelabel, sizeof(int), 1, fp);
			result = fwrite(&g->node[i].pred, sizeof(int), 1, fp);
			result = fwrite(&g->node[i].label, sizeof(int), 1, fp);
			result = fwrite(&g->node[i].pathval, sizeof(float), 1, fp);
			result = fwrite(&g->node[i].radius, sizeof(float), 1, fp);

			for (j = 0; j < g->nfeats; j++)
				result = fwrite(&g->node[i].feat[j], sizeof(float), 1, fp);
		}
	}

	for (i = 0; i < g->nnodes; i++)
		result = fwrite(&g->ordered_list_of_nodes[i], sizeof(int), 1, fp);

	fclose(fp);
}

int main(int argc, char **argv){
	fflush(stdout);
	fprintf(stdout, "\nProgram that executes the training phase of the OPF classifier\n");
	fprintf(stdout, "\nIf you have any problem, please contact: ");
	fprintf(stdout, "\n- alexandre.falcao@gmail.com");
	fprintf(stdout, "\n- papa.joaopaulo@gmail.com\n");
	fprintf(stdout, "\nLibOPF version 2.0 (2009)\n");
	fprintf(stdout, "\n"); fflush(stdout);

	if((argc != 3) && (argc != 2)){
		fprintf(stderr, "\nusage opf_train <P1> <P2>");
		fprintf(stderr, "\nP1: training set in the OPF file format");
		fprintf(stderr, "\nP2: precomputed distance file (leave it in blank if you are not using this resource)\n");
		exit(-1);
	}

	int n, i, debug = 1;
	char fileName[256];
	FILE *f = NULL;
	timer tic, toc;
	float time;
	size_t result;

	if(argc == 3) opf_PrecomputedDistance = 1;

	fprintf(stdout, "\nReading data file ..."); fflush(stdout);
	Subgraph *g = ReadSubgraph(argv[1]);
	fprintf(stdout, " OK"); fflush(stdout);

	if(opf_PrecomputedDistance)
		opf_DistanceValue = opf_ReadDistances(argv[2], &n);

	fprintf(stdout, "\nTraining OPF classifier ..."); fflush(stdout);
	gettimeofday(&tic,NULL); opf_OPFTraining(g); gettimeofday(&toc,NULL);
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nWriting classifier's model file ..."); fflush(stdout);
	opf_WriteModelFile(g, "classifier.opf");
	fprintf(stdout, " OK"); fflush(stdout);
	
	fprintf(stdout, "\nWriting output file ..."); fflush(stdout);
	sprintf(fileName,"%s.out",argv[1]);
	f = fopen(fileName,"w");
	for (i = 0; i < g->nnodes; i++)
		fprintf(f,"%d\n",g->node[i].label);
	fclose(f);
	fprintf(stdout, " OK"); fflush(stdout);

	if (debug) {
		fprintf(stdout, "\nWriting debug file ..."); fflush(stdout);
		sprintf(fileName,"%s.bug",argv[1]);
		f = fopen(fileName,"w");
		for (i = 0; i < g->nnodes; i++) {
			fprintf(f, "%d\n", g->node[i].label);
			fprintf(f, "%d\n", g->node[i].position);
			fprintf(f, "----------\n");
		}
		fclose(f);
		fprintf(stdout, " OK"); fflush(stdout);
	}

	fprintf(stdout, "\nDeallocating memory ..."); fflush(stdout);
	DestroySubgraph(&g);
	if(opf_PrecomputedDistance){
		for (i = 0; i < n; i++)
			free(opf_DistanceValue[i]);
		free(opf_DistanceValue);
	}
	fprintf(stdout, " OK\n");

	time = ((toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001)/1000.0;
	fprintf(stdout, "\nTraining time: %f seconds\n", time); fflush(stdout);

	sprintf(fileName,"%s.time",argv[1]);
	f = fopen(fileName,"a");
	result = fprintf(f,"%f\n",time);
	fclose(f);

	return 0;
}