#include "OPF.h"

void opf_OPFClassifyingRandom(Subgraph *sgtrain, Subgraph *sg, float **minLabel)
{
  int i, j, k, l, p, label = -1;
  float tmp, weight, minCost, sumCost;
  int nlabels = sgtrain->nlabels;
  //matrix to store scores for each object and class
  //float **minLabel = (float **) calloc(sg->nnodes, sizeof(float *));

  // for each test set node
  for (i = 0; i < sg->nnodes; i++)  
  {
    // allocate row for element i and fill with max float value
    //minLabel[i] = (float *) calloc(nlabels, sizeof(float));
    for (p=0; p < nlabels; p++) { minLabel[i][p]=FLT_MAX; }
    
    j       = 0;
    k       = sgtrain->ordered_list_of_nodes[j];
    
    if(!opf_PrecomputedDistance)
      weight = opf_ArcWeight(sgtrain->node[k].feat,sg->node[i].feat,sg->nfeats);
    else
      weight = opf_DistanceValue[sgtrain->node[k].position][sg->node[i].position];
    
    minCost = MAX(sgtrain->node[k].pathval, weight);
    label   = sgtrain->node[k].label;
    minLabel[i][label-1] = minCost;
    
    while ((j < sgtrain->nnodes-1) &&
    (minCost > sgtrain->node[sgtrain->ordered_list_of_nodes[j+1]].pathval)){

      l  = sgtrain->ordered_list_of_nodes[j+1];

      if(!opf_PrecomputedDistance)
	weight = opf_ArcWeight(sgtrain->node[l].feat,sg->node[i].feat,sg->nfeats);
      else
	weight = opf_DistanceValue[sgtrain->node[l].position][sg->node[i].position];
      tmp = MAX(sgtrain->node[l].pathval, weight);
      
      if (tmp < minLabel[i][sgtrain->node[l].label-1]) {
        minLabel[i][sgtrain->node[l].label-1] = tmp;
	//printf("(%d) %d %.4f\n", j, sgtrain->node[l].label, tmp);
      }

      if (tmp < minCost){
	minCost = tmp;
	label = sgtrain->node[l].label;
      }
      j++;
      k  = l;
    }
    sumCost = 0.0;
    for (p=0; p < nlabels; p++) { 
      sumCost+= minLabel[i][p];
    }
    for (p=0; p < nlabels; p++) { 
      minLabel[i][p] = (1.0-(minLabel[i][p]/(float)sumCost))/(nlabels-1);
    }
    sg->node[i].label = label;
  }
}


int main(int argc, char **argv){
	fflush(stdout);
	fprintf(stdout, "\nProgram that returns the score for each class using the OPF classifier\n");
	fprintf(stdout, "\nIf you have any problem, please contact: ");
	fprintf(stdout, "\n- alexandre.falcao@gmail.com");
	fprintf(stdout, "\n- papa.joaopaulo@gmail.com\n");
	fprintf(stdout, "\nLibOPF version 2.0 (2009)\n");
	fprintf(stdout, "\n"); fflush(stdout);

	if((argc != 3) && (argc != 2)){
		fprintf(stderr, "\nusage opf_classifyscore <P1> <P2>");
		fprintf(stderr, "\nP1: test set in the OPF file format");
		fprintf(stderr, "\nP2: precomputed distance file (leave it in blank if you are not using this resource\n");
		exit(-1);
	}

	int n,i,p;
	int nlabels, nnodes;
	float time;
	//float **opf_DistanceValue;
	float **scoreMatrix;
	char fileName[256];
	char fileNameS[256];
	char fileNameT[256];
	FILE *f = NULL;
	FILE *fS = NULL;
	FILE *fT = NULL;
	timer tic, toc;
	size_t result;

	if(argc == 3) opf_PrecomputedDistance = 1;
	fprintf(stdout, "\nReading data files ..."); fflush(stdout);
	Subgraph *gTest = ReadSubgraph(argv[1]), *gTrain = opf_ReadModelFile("classifier.opf");
	fprintf(stdout, " OK"); fflush(stdout);

	if (opf_PrecomputedDistance)
		opf_DistanceValue = opf_ReadDistances(argv[2], &n);

	// for each test set node
	nlabels = gTest->nlabels;
	nnodes = gTest->nnodes;
	scoreMatrix = (float **) malloc(nnodes*sizeof(float *));
	for (i=0; i<nnodes; i++) {
	    scoreMatrix[i] = (float *) malloc(nlabels*sizeof(float));
	}

	fprintf(stdout, "\nClassifying test set ..."); fflush(stdout);
	gettimeofday(&tic,NULL);

	opf_OPFClassifyingScore(gTrain, gTest, scoreMatrix); gettimeofday(&toc,NULL);
	fprintf(stdout, " OK"); fflush(stdout);
	
// 	printf("\nScores\n: ");
// 	for (i=0; i < gTest->nnodes; i++) {
// 	   printf("\n[%d]: ", i+1);
// 	   float sumCosts = 0.0;
// 	   for (p=0; p < nlabels; p++) {
// 	     printf("%.4f\t", scoreMatrix[i][p]);
// 	     sumCosts += scoreMatrix[i][p];
// 	   }
// 	   printf("%.4f", sumCosts);
// 	}

	fprintf(stdout, "\nWriting output files ..."); fflush(stdout);
	sprintf(fileName,"%s.out",argv[1]);
	sprintf(fileNameS,"%s_scores.out",argv[1]);
	sprintf(fileNameT,"%s_true.out",argv[1]);
	f = fopen(fileName,"w");
	fS= fopen(fileNameS,"w");
	fT= fopen(fileNameT,"w");
	for (i = 0; i < nnodes; i++){
		//fprintf(f,"%d %d\n",gTest->node[i].label,gTest->node[i].truelabel);
		fprintf(f,"%d\n",gTest->node[i].label);
		fprintf(fT,"%d\n",gTest->node[i].truelabel);
		for (p=0; p < nlabels; p++) {
		    fprintf(fS, "%.4f\t", scoreMatrix[i][p]);
		}
		fprintf(fS,"\n");
	}
	fclose(f);
	fclose(fS);
	fclose(fT);
	fprintf(stdout, " OK"); fflush(stdout);

	fprintf(stdout, "\nDeallocating memory ...");
	DestroySubgraph(&gTrain);
	DestroySubgraph(&gTest);
	
 	for (i=0; i<nnodes; i++) 
 	    free(scoreMatrix[i]);
 	free(scoreMatrix);
 	
	if(opf_PrecomputedDistance){
		for (i = 0; i < n; i++)
			free(opf_DistanceValue[i]);
		free(opf_DistanceValue);
	}
	fprintf(stdout, " OK\n");

	time = ((toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001)/1000.0;
	fprintf(stdout, "\nTesting time: %f seconds\n", time); fflush(stdout);

	sprintf(fileName,"%s.time",argv[1]);
	f = fopen(fileName,"a");
	result = fprintf(f,"%f\n",time);
	fclose(f);

	return 0;
}
