/*
 * Nome: Isadora Rossi			Data: 08/09/2011
 * Descricao: 
 */
#include "OPF.h"
#include <unistd.h>

int main(int argc, char *argv[]){
	if(argc != 6){
		fprintf(stderr,"\n*** ICPR 2012 - Code Supplementary Material (Isadora Rossi, Moacir Ponti Jr)\n");
		fprintf(stderr, "\nusage opf_repeat <P1> <P2> <P3> <P4> <P5> <P6>");
		fprintf(stderr, "\nP1: input dataset in the OPF file format");
		fprintf(stderr, "\nP2: percentage for the training set size [0,1]");
// 		fprintf(stderr, "\nP3: percentage for the evaluation set size [0,1] (leave 0 in the case of no learning)");
		fprintf(stderr, "\nP3: percentage for the test set size [0,1]");
		fprintf(stderr, "\nP4: normalize features? 1 - Yes  0 - No");			
		fprintf(stderr, "\nP5: number of REPETITIONS\n\n");	
		exit(-1);
	}
	Subgraph *g = NULL, *gAux = NULL, *gTraining = NULL, *gEvaluating = NULL, *gTesting = NULL;
	float training_p = atof(argv[2]);//porcentagem de treinamento
	float evaluating_p = 0; //atof(argv[3]); ;//porcentagem de avaliação
	float testing_p = atof(argv[3]);//porcentagem de teste
	
	int qtd = atoi(argv[5]); 
	int normalizef = atoi(argv[4]); 
	
	int cont1;
	
	float averageacc = 0.0f;
	float *accuracies = calloc(qtd, sizeof(float));

	timer tic, toc;
	float time_i = 1.0, time_total = 0.0;

	fprintf(stdout,"ICPR 2012 - Code Supplementary Material (Isadora Rossi, Moacir Ponti Jr)\n");
	fprintf(stdout,"OPF single classifier\n");
	fprintf(stdout,"Holdout (%.2f-%.2f-%.2f) results with %d repetitions:\n", training_p, evaluating_p, testing_p, qtd);
	fflush(stdout);
	g = ReadSubgraph(argv[1]);//faz um grafo do arquivo original	
	
	for(cont1 = 0 ; cont1 < qtd ; cont1++){//laco para a quantidade de porcentagens testadas
		sleep(1);
			
		if (normalizef == 1)
			opf_NormalizeFeatures(g);
	
		if (evaluating_p == 0.0) {
		    opf_SplitSubgraph(g, &gTraining, &gTesting, training_p);//particionar o g em 
		    gettimeofday(&tic,NULL); 
		    opf_OPFTraining(gTraining);
		}
		else {
		    //Faz um split no grafo original	
		    opf_SplitSubgraph(g, &gAux, &gTesting, training_p+evaluating_p);
		    
		    opf_SplitSubgraph(gAux, &gTraining, &gEvaluating, training_p/(training_p+evaluating_p));
		    
		    gettimeofday(&tic,NULL); 
		    opf_OPFLearning(&gTraining, &gEvaluating);
		}
			
		opf_OPFClassifying(gTraining, gTesting);

		gettimeofday(&toc,NULL);
		time_i = ((toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001)/1000.0;
		time_total += time_i;
			
		accuracies[cont1]= opf_Accuracy(gTesting)*100;
			
		averageacc += accuracies[cont1];
		//usleep(1000000);//tempo de espera para mudar o relogio
		fprintf(stdout,"%.2f\n", accuracies[cont1]);
		fflush(stdout);
			
		if (evaluating_p == 0.0) {
		    DestroySubgraph(&gAux);
		    DestroySubgraph(&gEvaluating);
		}
		DestroySubgraph(&gTesting);
		DestroySubgraph(&gTraining);
		/*Modifica as condições do teste*/
		
			//training_p -= 0.1;
			//testing_p += 0.1;
	}
	averageacc = (averageacc/(float)qtd);
	time_total = time_total/(float)qtd;
	float stdacc = 0.0f;
	for(cont1 = 0 ; cont1 < qtd ; cont1++){
	    float temp = (accuracies[cont1] - averageacc);
	    stdacc = temp*temp;
	}
	stdacc = sqrt(stdacc/(float)qtd);
	fprintf(stdout, "\n------------------------------------------------------");
	fprintf(stdout, "\nAverage accuracy after %d repetitions: %.2f +- %.2f", qtd, averageacc, stdacc);
	fprintf(stdout, "\nAverage time spent on each iteration: %.6f ms\n\n", time_total);
	fflush(stdout);
	
	free(accuracies);
	DestroySubgraph(&g);
	return 0;
}
