#include "OPF.h"

#define MAXITERS 1000
#define FALSE 0
#define TRUE !(FALSE)

void getLabels(Subgraph *data, int *labels, int *centroids, int k) {
	int i, j, minid;
	float weight, minw;

	for(i = 0; i < data->nnodes; i++) {
		minid = -1;
		minw = FLT_MAX;

		for(j = 0; j < k; j++) {
			// Get weight 
			weight = opf_ArcWeight(data->node[centroids[j]].feat, data->node[i].feat, data->nfeats);

			// Verify if minimum and update
			if(minw > weight) {
				minid = centroids[j];
				minw = weight;
			}
		}

		// Set the label of node[i] as centroid index
		labels[i] = minid;
	}
}

int* getCentroids(Subgraph *data, int *labels, int *centroids, int k) {
	int i, j, f, minid;
	float weight, minw;
	SNode *mean;
	int *count;

	// Initialize means verctor for each centroid
	count = AllocIntArray(k);
	mean = (SNode*) calloc(k, sizeof(SNode));
	for(i = 0; i < k; i++) {
		count[i] = 0;
		mean[i].feat = AllocFloatArray(data->nfeats);
		for(j = 0; j < data->nfeats; j++) mean[i].feat[j] = 0.0;
	}

	// Calculate the means for each centroid
	for(i = 0; i < k; i++) { // for each centroid
		for(j = 0; j < data->nnodes; j++) { // and each node
			if(labels[j] == centroids[i]) { // verify which if is cluster 'i'
				for(f = 0; f < data->nfeats; f++) { // calculate mean
					mean[i].feat[f] += data->node[j].feat[f];
					count[i]++;
				}
			}
		}
	}

	// Assign new centroids
	for(i = 0; i < k; i++) {
		for(f = 0; f < data->nfeats; f++)
			mean[i].feat[f] /= count[i];

		minid = -1;
		minw = INT_MAX;
		for(j = 0; j < data->nnodes; j++) {
			if(labels[j] == centroids[i]) {
				// Get weight compared to mean
				weight = opf_ArcWeight(mean[i].feat, data->node[j].feat, data->nfeats);

				// Verify if minimum and update
				if(minw > weight) {
					minid = j;
					minw = weight;
				}
			}
		}
		centroids[i] =  minid;
	}

	free(count);
	for(i = 0; i < k; i++) free(mean[i].feat);
	free(mean);
}

int shouldStop(int *old, int *cur, int k, int iters) {
	int i;
	// If reached max iterations should stop
	if(iters >= MAXITERS) return TRUE;

	for(i = 0; i < k; i++)
		if(old[i] != cur[i]) return FALSE; // If there is some change shouldn't stop
	return TRUE;
}

void cpyarr(int *a, int *b, int n) {
	int i;
	for(i = 0; i < n; i++) {
		a[i] = b[i];
	}
}

// Código original disponibilizado por Prof. Moacir Ponti
Subgraph* opf_OPFTrainKMeans(Subgraph *sgTrain, int k) {
	Subgraph *sgkmeans = NULL;
	int *centroids, *oldCentroids, *labels;
	int i, j, n, iters = 0;

	centroids = AllocIntArray(k);
	oldCentroids = AllocIntArray(k);
	labels = AllocIntArray(sgTrain->nnodes);

	sgkmeans = CreateSubgraph(k);
	sgkmeans->nnodes = k;
	sgkmeans->nfeats = sgTrain->nfeats;// copia o numero dos atributos
	for (i = 0; i < k; i++)// aloca a quantidade de atributos
        sgkmeans->node[i].feat = AllocFloatArray(sgTrain->nfeats);
	
	// initialize centroids as k-first nodes with smallest pathval
	for(i = 0; i < k; i++) centroids[i] = rand()%sgTrain->nnodes;

	fprintf(stdout, " %d", k); fflush(stdout);
	while(!shouldStop(oldCentroids, centroids, k, iters)){
		cpyarr(oldCentroids, centroids, k);
		iters++;

		// Set the labels according to closest centroids
		getLabels(sgTrain, labels, centroids, k);

		// For each cluster calculate new centroids 
		getCentroids(sgTrain, labels, centroids, k);
	}

	// Set centroids as new subgraph
	for(i = 0; i < k; i++) {
        j = centroids[i];
		CopySNode(&(sgkmeans->node[i]), &(sgTrain->node[j]), sgTrain->nfeats);
    }

	free(labels);
	free(centroids);
	free(oldCentroids);

    return sgkmeans;
}

int main(int argc, char **argv) {
	fflush(stdout);
	fprintf(stdout, "\nProgram that executes the training phase of the Kmeans classifier\n");
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

	int n, i, j, k, debug = 1;
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

	fprintf(stdout, "\nGetting K-Mean Nodes ..."); fflush(stdout);
	sprintf(fileName, "%s.nprotos", argv[1]);
	f = fopen(fileName, "r");
	i = fscanf(f, "%d\n", &k);
	fclose(f);
	Subgraph *gKMeans = opf_OPFTrainKMeans(gTrain, k);
	opf_OPFTraining(gKMeans);
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nWriting classifier's model file ..."); fflush(stdout);
	opf_WriteModelFile(gKMeans, "classifier.opf");
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nWriting output file ..."); fflush(stdout);
	sprintf(fileName,"%s.out",argv[1]);
	f = fopen(fileName,"w");
	for (i = 0; i < gKMeans->nnodes; i++)
		fprintf(f,"%d\n",gKMeans->node[i].label);
	fclose(f);
	fprintf(stdout, " OK"); fflush(stdout);

	if (debug) {
		fprintf(stdout, "\nWriting debug file ..."); fflush(stdout);
		sprintf(fileName, "%s.bug", argv[1]);
		f = fopen(fileName,"w");
		for (i = 0; i < gKMeans->nnodes; i++) {
			fprintf(f, "%d\n",gKMeans->node[i].label);
			fprintf(f, "%d\n",gKMeans->node[i].position);
			fprintf(f, "%d\n", gKMeans->ordered_list_of_nodes[i]);
			fprintf(f, "----------\n");
		}
		fclose(f);
		fprintf(stdout, " OK"); fflush(stdout);
	}

	fprintf(stdout, "\nDeallocating memory ..."); fflush(stdout);
	DestroySubgraph(&gTrain);
	DestroySubgraph(&gKMeans);
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
