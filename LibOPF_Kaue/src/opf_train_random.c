#include "OPF.h"

typedef struct {
	float pathval;
	int id;
} sortsg;

int compare(const void *a, const void *b) {
	sortsg *x = (sortsg*) a;
	sortsg *y = (sortsg*) b;
	return (x->pathval > y->pathval);
}

int* shuffle(int n) {
	int *array = NULL;
	int i, randi, aux;

	array = AllocIntArray(n);

	for (i = 0; i < n; i++) array[i] = i;
	for (i = 0; i < n; i++) {
		randi = rand() % n;
		
		aux = array[i];
		array[i] = array[randi];
		array[randi] = aux;
	}

	return array;
}

// Código original disponibilizado por Prof. Moacir Ponti
Subgraph* opf_OPFTrainRandom(Subgraph *sgTrain, int nrand) {
	Subgraph *sgrand = NULL;
	int *randseq = NULL;
	sortsg ordered_index[nrand];
    int i, j, n;

    sgrand = CreateSubgraph(nrand);// cria uma subgraph
    sgrand->nlabels = sgTrain->nlabels;// copia o numero de rotulos
    sgrand->nnodes = nrand;

    sgrand->nfeats = sgTrain->nfeats;// copia o numero dos atributos
    for (i = 0; i < nrand; i++)//aloca a quantidade de atributos
        sgrand->node[i].feat = AllocFloatArray(sgTrain->nfeats);
   
	srand(time(NULL));
	randseq = shuffle(sgTrain->nnodes);

	fprintf(stdout, "%d", nrand);
	for (i = 0; i < nrand; i++) {
		j = randseq[i];

		for (n = 0; n < sgTrain->nfeats; n++)
            sgrand->node[i].feat[n] = sgTrain->node[j].feat[n];

		// copia o rotulo e rotulo verdadeiro(supervisionado)
		sgrand->node[i].label = sgTrain->node[j].label;
		sgrand->node[i].truelabel = sgTrain->node[j].truelabel;
		// copia a posicao
		sgrand->node[i].position = sgTrain->node[j].position;
		
		ordered_index[i].id = i;
		ordered_index[i].pathval = sgTrain->node[j].pathval;
	}

	// sort and set the ordered list of nodes
	qsort(ordered_index, nrand, sizeof(sortsg), compare);
	for (i = 0; i < nrand; i++)
		sgrand->ordered_list_of_nodes[i] = ordered_index[i].id;

	free(randseq);

    return sgrand;
}

int main(int argc, char **argv) {
	fflush(stdout);
	fprintf(stdout, "\nProgram that executes the training phase of the Random classifier\n");
	fprintf(stdout, "\nIf you have any problem, please contact: ");
	fprintf(stdout, "\n- alexandre.falcao@gmail.com");
	fprintf(stdout, "\n- papa.joaopaulo@gmail.com\n");
	fprintf(stdout, "\nLibOPF version 2.0 (2009)\n");
	fprintf(stdout, "\n"); fflush(stdout);

	if((argc != 3) && (argc != 2)) {
		fprintf(stderr, "\nusage opf_train <P1> <P2>");
		fprintf(stderr, "\nP1: training set in the OPF file format");
		fprintf(stderr, "\nP2: precomputed distance file (leave it in blank if you are not using this resource)\n");
		exit(-1);
	}

	int n, i, j, nrand, debug = 1;
	char fileName[256];
	FILE *f = NULL;
	timer tic, toc;
	float time;
	size_t result;

	if(argc == 3) opf_PrecomputedDistance = 1;

	fprintf(stdout, "\nReading data file ..."); fflush(stdout);
	Subgraph *gTrain = ReadSubgraph(argv[1]);
	fprintf(stdout, " OK"); fflush(stdout);

	if(opf_PrecomputedDistance)
		opf_DistanceValue = opf_ReadDistances(argv[2], &n);

	fprintf(stdout, "\nTraining OPF classifier ..."); fflush(stdout);
	gettimeofday(&tic,NULL); opf_OPFTraining(gTrain); gettimeofday(&toc,NULL);
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nGetting N-Random Nodes ..."); fflush(stdout);
	sprintf(fileName, "%s.nprotos", argv[1]);
	f = fopen(fileName, "r");
	fscanf(f, "%d\n", &nrand);
	fclose(f);
	Subgraph *gRandom = opf_OPFTrainRandom(gTrain, nrand);
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nWriting classifier's model file ..."); fflush(stdout);
	opf_WriteModelFile(gRandom, "classifier.opf");
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nWriting output file ..."); fflush(stdout);
	sprintf(fileName,"%s.out",argv[1]);
	f = fopen(fileName,"w");
	for (i = 0; i < gRandom->nnodes; i++)
		fprintf(f,"%d\n",gRandom->node[i].label);
	fclose(f);
	fprintf(stdout, " OK"); fflush(stdout);

	if (debug) {
		fprintf(stdout, "\nWriting debug file ..."); fflush(stdout);
		sprintf(fileName,"%s.bug",argv[1]);
		f = fopen(fileName,"w");
		for (i = 0; i < gRandom->nnodes; i++) {
			fprintf(f, "%d\n",gRandom->node[i].label);
			fprintf(f, "%d\n",gRandom->node[i].position);
			fprintf(f, "%d\n", gRandom->ordered_list_of_nodes[i]);
			fprintf(f, "----------\n");
		}
		fclose(f);
		fprintf(stdout, " OK"); fflush(stdout);
	}

	fprintf(stdout, "\nDeallocating memory ..."); fflush(stdout);
	DestroySubgraph(&gTrain);
	DestroySubgraph(&gRandom);
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
