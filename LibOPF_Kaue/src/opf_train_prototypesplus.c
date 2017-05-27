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

// Código original disponibilizado por Prof. Moacir Ponti
Subgraph* opf_OPFTrainPrototypesPlus(Subgraph *sgTrain, int *nprotos, float rate) {
	Subgraph *protos = NULL;
	sortsg *ordered_index = NULL;
    int i, j, k, n;
    *nprotos = 0;

    //Conta quantos prototipos existem
    for(i = 0; i < sgTrain->nnodes; i++) 
        if(sgTrain->node[i].status == 1) (*nprotos)++;

    if((*nprotos) > sgTrain->nnodes*rate)
		(*nprotos) = sgTrain->nnodes*rate;

    if((*nprotos) >= sgTrain->nnodes/2) {
        (*nprotos) = sgTrain->nnodes;
        return sgTrain;
    }

	fprintf(stdout, "%d/%d", (*nprotos)*2, sgTrain->nnodes);

    ordered_index = (sortsg*) calloc((*nprotos)*2, sizeof(sortsg));
    if(ordered_index == NULL)
        Error(MSG1, "Allocation of memory for struct");

    protos = CreateSubgraph((*nprotos)*2);// cria uma subgraph
    protos->nlabels = sgTrain->nlabels;// copia o numero de rotulos
    protos->nnodes = (*nprotos)*2;

    protos->nfeats = sgTrain->nfeats;// copia o numero dos atributos
    for (i = 0; i < (*nprotos)*2; i++)// aloca a quantidade de atributos
        protos->node[i].feat = AllocFloatArray(sgTrain->nfeats);

    k = 0;
	for(i = 0; i < (*nprotos); i++) {
        j = sgTrain->ordered_list_of_nodes[i];
        
        for (n = 0; n < sgTrain->nfeats; n++)
            protos->node[k].feat[n] = sgTrain->node[j].feat[n];
        
        // copia o rotulo e rotulo verdadeiro(supervisionado)
        protos->node[k].label = sgTrain->node[j].label;
        protos->node[k].truelabel = sgTrain->node[j].truelabel;
        // copia a posicao
        protos->node[k].position = sgTrain->node[j].position;

        ordered_index[k].id = k;
		ordered_index[k].pathval = sgTrain->node[j].pathval;

        k++;
    }
    for(i = sgTrain->nnodes - (*nprotos); i < (*nprotos); i++) {
        j = sgTrain->ordered_list_of_nodes[i];

        for (n = 0; n < sgTrain->nfeats; n++)
            protos->node[k].feat[n] = sgTrain->node[j].feat[n];
        
        // copia o rotulo e rotulo verdadeiro(supervisionado)
        protos->node[k].label = sgTrain->node[j].label;
        protos->node[k].truelabel = sgTrain->node[j].truelabel;
        // copia a posicao
        protos->node[k].position = sgTrain->node[j].position;

        ordered_index[k].id = k;
		ordered_index[k].pathval = sgTrain->node[j].pathval;

        k++;
    }

    (*nprotos) *= 2;

	// sort and set the ordered list of nodes
	qsort(ordered_index, (*nprotos), sizeof(sortsg), compare);
	for (i = 0; i < (*nprotos); i++)
		protos->ordered_list_of_nodes[i] = ordered_index[i].id;


    free(ordered_index);

    return protos;
}

int main(int argc, char **argv) {
	fflush(stdout);
	fprintf(stdout, "\nProgram that executes the training phase of the Prototypes Plus classifier\n");
	fprintf(stdout, "\nIf you have any problem, please contact: ");
	fprintf(stdout, "\n- alexandre.falcao@gmail.com");
	fprintf(stdout, "\n- papa.joaopaulo@gmail.com\n");
	fprintf(stdout, "\nLibOPF version 2.0 (2009)\n");
	fprintf(stdout, "\n"); fflush(stdout);

	if((argc != 4) && (argc != 3)){
		fprintf(stderr, "\nusage opf_train <P1> <P2> <P3>");
		fprintf(stderr, "\nP1: training set in the OPF file format");
		fprintf(stderr, "\nP2: percentage of the total set that is going to be used by the algorithm (0,1]\n");
		fprintf(stderr, "\nP3: precomputed distance file (leave it in blank if you are not using this resource)\n");
		exit(-1);
	}

	int n, i, nprotos, debug = 1;
	char fileName[256];
	FILE *f = NULL;
	timer tic, toc;
	float time;
	size_t result;

	if(argc == 4) opf_PrecomputedDistance = 1;

	fprintf(stdout, "\nReading data file ..."); fflush(stdout);
	Subgraph *gTrain = ReadSubgraph(argv[1]);
	fprintf(stdout, " OK"); fflush(stdout);

	if(opf_PrecomputedDistance)
		opf_DistanceValue = opf_ReadDistances(argv[3], &n);

	fprintf(stdout, "\nTraining OPF classifier ..."); fflush(stdout);
	gettimeofday(&tic,NULL); opf_OPFTraining(gTrain); gettimeofday(&toc,NULL);
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nGetting Prototypes ..."); fflush(stdout);
	nprotos = 0;
	Subgraph *gPrototypes = opf_OPFTrainPrototypesPlus(gTrain, &nprotos, atof(argv[2]));
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nWriting classifier's model file ..."); fflush(stdout);
	opf_WriteModelFile(gPrototypes, "classifier.opf");
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nWriting output file ..."); fflush(stdout);
	sprintf(fileName,"%s.out",argv[1]);
	f = fopen(fileName,"w");
	for (i = 0; i < gPrototypes->nnodes; i++)
		fprintf(f,"%d\n",gPrototypes->node[i].label);
	fclose(f);
	fprintf(stdout, " OK"); fflush(stdout);

	if (debug) {
		fprintf(stdout, "\nWriting debug file ..."); fflush(stdout);
		sprintf(fileName,"%s.bug",argv[1]);
		f = fopen(fileName,"w");
		for (i = 0; i < gPrototypes->nnodes; i++) {
			fprintf(f, "%d\n",gPrototypes->node[i].label);
			fprintf(f, "%d\n",gPrototypes->node[i].position);
			fprintf(f, "%d\n", gPrototypes->ordered_list_of_nodes[i]);
			fprintf(f, "----------\n");
		}
		fclose(f);
		fprintf(stdout, " OK"); fflush(stdout);
	}

	fprintf(stdout, "\nDeallocating memory ..."); fflush(stdout);
	DestroySubgraph(&gTrain);
	DestroySubgraph(&gPrototypes);
	if(opf_PrecomputedDistance){
		for (i = 0; i < n; i++)
			free(opf_DistanceValue[i]);
		free(opf_DistanceValue);
	}
	fprintf(stdout, " OK\n");

	time = ((toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001)/1000.0;
	fprintf(stdout, "\nTraining time: %f seconds\n", time); fflush(stdout);

	sprintf(fileName, "%s.nprotos", argv[1]);
	f = fopen(fileName, "w");
	fprintf(f, "%d\n", nprotos);
	fclose(f);

	sprintf(fileName,"%s.time",argv[1]);
	f = fopen(fileName,"a");
	result = fprintf(f,"%f\n",time);
	fclose(f);

	return 0;
}
