
#include "OPF.h"
/*
 * O IVote vai construindo classificadores, um após o outro. Para isso
ele seleciona amostras de treinamento com base no teste feito com os
out-of-bag classifiers. Veja se entende:

0) Particione o conjunto de dados em X = treinamento, Y = teste e A =
avaliação (esse será usado para medir o erro dos classificadores)

1) Comece com o classificador h_0 sorteando M amostras de treinamento
e montando um classificador

2) Avalie o erro e(0) usando A (classifique os elementos de A usando
h_0 e verifique o erro)

2) Repita para i=1...K classificadores

2.1) Inicialize T_i = vazio

2.2) Enquanto o número de elementos em T_i for menor que M:

2.2.1) sorteie um elemento x_j do conjunto de treinamento e verifique
se ele já existe nos classificadores h_p (onde p < i)
2.2.2) classifique x_j usando TODOS os out-of-bag classifiers e
combine os resultados usando votação por maioria
2.2.3) se após a combinação o rótulo estiver errado, adicione x_j ao
conjunto T_i
2.2.4) caso contrário calcule adicione o elemento com probabilidade p
= e_{i-1} / ( 1- e_{i-1} )

2.3) Treine o classificador h_i usando T_i

2.4) Avalie e(i) = (erro de h_i) usando o conjunto A

3) Teste o ensemble H = (h_0, ... h_K) usando o conjunto Y,
classificando cada objeto e tomando por decisão final a maioria.


Repare que, como opção, o erro também poderia ser obtido por meio dos
out-of-bag classifiers, ao invés de usar um conjunto de avaliação A.
 * 
 #include "OPF.h"
/*
 * O IVote vai construindo classificadores, um após o outro. Para isso
ele seleciona amostras de treinamento com base no teste feito com os
out-of-bag classifiers. Veja se entende:

0) Particione o conjunto de dados em X = treinamento, Y = teste e A =
avaliação (esse será usado para medir o erro dos classificadores)

1) Comece com o classificador h_0 sorteando M amostras de treinamento
e montando um classificador

2) Avalie o erro e(0) usando A (classifique os elementos de A usando
h_0 e verifique o erro)

2) Repita para i=1...K classificadores

2.1) Inicialize T_i = vazio

2.2) Enquanto o número de elementos em T_i for menor que M:

2.2.1) sorteie um elemento x_j do conjunto de treinamento e verifique
se ele já existe nos classificadores h_p (onde p < i)
2.2.2) classifique x_j usando TODOS os out-of-bag classifiers e
combine os resultados usando votação por maioria
2.2.3) se após a combinação o rótulo estiver errado, adicione x_j ao
conjunto T_i
2.2.4) caso contrário calcule adicione o elemento com probabilidade p
= e_{i-1} / ( 1- e_{i-1} )

2.3) Treine o classificador h_i usando T_i

2.4) Avalie e(i) = (erro de h_i) usando o conjunto A

3) Teste o ensemble H = (h_0, ... h_K) usando o conjunto Y,
classificando cada objeto e tomando por decisão final a maioria.


Repare que, como opção, o erro também poderia ser obtido por meio dos
out-of-bag classifiers, ao invés de usar um conjunto de avaliação A.
 * 
 * */

//retorna apenas um classificador sem nenhuma amostra
void empty_ensemble(Subgraph *g, Subgraph *gTraining, int posicao,int k, Subgraph **out){
	
	
	//Subgraph *out = (Subgraph *)calloc(k, sizeof(Subgraph));
	
	int i,j;


	//Subamostragem
	int n = g->nnodes;
	int m = (n/2)/k;	
	

    printf("alocar\n");
	out[posicao] = CreateSubgraph(m);//cria um novo	
	out[posicao]->nfeats = g->nfeats;//copia o numero dos atributos	
	out[posicao]->nlabels = g->nlabels;//copia o numero de rotulos
	
		for (j = 0; j <m; j++)//aloca a quantidade de atributos
			out[posicao]->node[j].feat = AllocFloatArray(out[posicao]->nfeats);
			
			printf("Novo grafo vazio criado!\n");
	
	
}

//computa o erro do classificador
float opf_error(Subgraph *sg){
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

	return(Acc);
}

//classifica apenas um objeto 
//sgtrain eh o conjunto de treinamento
//out_of_bag eh a amostra que nao contem o no que esta no conjunto de treinamento
//o node eh marcado por sua posicao_node
int one_objectClassifying(Subgraph *sgtrain, Subgraph *out_of_bag, int posicao_node){
	int i, j, k, l, label = -1;
	float tmp, weight, minCost;

	j       = 0;
	//k       = sgtrain->ordered_list_of_nodes[j];
	
	//printf("Ordered_list_of_nodes %d\n",k);
	
	if(!opf_PrecomputedDistance)
		weight = opf_ArcWeight(sgtrain->node[k].feat,out_of_bag->node[posicao_node].feat,out_of_bag->nfeats);
	else
		weight = opf_DistanceValue[sgtrain->node[k].position][out_of_bag->node[posicao_node].position];

	minCost = MAX(sgtrain->node[k].pathval, weight);
	label   = sgtrain->node[k].label;

	while((j < sgtrain->nnodes-1)&&	(minCost > sgtrain->node[sgtrain->ordered_list_of_nodes[j+1]].pathval)){

		l  = sgtrain->ordered_list_of_nodes[j+1];

		if(!opf_PrecomputedDistance)
			weight = opf_ArcWeight(sgtrain->node[l].feat,out_of_bag->node[posicao_node].feat,out_of_bag->nfeats);
		else
			weight = opf_DistanceValue[sgtrain->node[l].position][out_of_bag->node[posicao_node].position];
		tmp = MAX(sgtrain->node[l].pathval, weight);
		if(tmp < minCost){
			minCost = tmp;
			label = sgtrain->node[l].label;
		}
		j++;
		k  = l;
	}
	out_of_bag->node[posicao_node].label = label;
	return label;
}


Subgraph **opf_ivotetrain(Subgraph *g,Subgraph *gTraining, Subgraph *gTesting, Subgraph *gEvaluating, int k){
	int i, j,aux,l,n, maior,randomico,nclassificadores, num_of_misclassified = 0;
	Subgraph **out = (Subgraph **)calloc(k, sizeof(Subgraph*));
	int total = g->nnodes;
	int subamostragem = (total/2)/k;	
	float R;	
	float erro;
	int ON = g->nnodes;
	int N = (ON/2)/k;
	
	//aqui fará o controle de quais objetos já foram inserido em classificadores antigos
	for (i = 0; i < gTraining->nnodes; i++)
		gTraining->node[i].relevant = 0;
		
	//vetor que calcula os erros dos classificadores
	float error[k];
	int vet_label[g->nlabels];
		
	char fileName[100];
	char count[100];	
	int label = 0;
	
	opf_PrecomputedDistance = 0;//distancia euclidiana
	
	//cria o primeiro classificador vazio
	empty_ensemble(g, gTraining,  0,k, out);	

	//preenche esse unico classificador com numeros randomicos com reposicao (bagging)	
		for (i = 0; i < N; i++){//para cada no existente nesse grafo		
			j = RandomInteger(0,N-1);//escolhe um no randomico
			for (n = 0; n < g->nfeats; n++)//percorre todos os atributos
				out[0]->node[i].feat[n] = g->node[j].feat[n];//copiando para o novo grafo os atributos
			out[0]->node[i].truelabel = g->node[j].truelabel;//copia o rotulo verdadeiro(supervisionado)
			out[0]->node[i].position = g->node[i].position;
		}
	
	
	printf("Primeiro grafo preenchido com sucesso!\n");
/*	
	for (i = 0; i < N; i++){
		printf("Objeto[%d] =  ",i);
		for (n = 0; n < g->nfeats; n++)
			printf("%lf ",out[0]->node[i].feat[n]);
		printf("\n");	
	}*/
	
	//classifico o primeiro classificador
	opf_OPFTraining(out[0]);
	opf_OPFClassifying(out[0],gEvaluating);
	
	WriteSubgraph(out[0],"Classifier0");//grava o grafo no disco o primeiro classificador

	
	
	error[0] = opf_error(gEvaluating);//grarda o primeiro valor de erro no vetor
	printf("Erro[0] = %f\n",error[0]);

/*
	for(i=1; i<k; i++){
		//cria um novo classificador*/
		empty_ensemble(g, gTraining,  i,k, out);	
		printf("ok empty!!\n");
		//for(j=1; j<n; j++){
			randomico = RandomInteger(0,N-1);//sorteia um numero no conjunto de treinamento
			
			//verifica se os classificadores anteriores tem esse objeto			
			if(gTraining->node[randomico].relevant != 1){//se nenhum tiver esse objeto			
				//for(aux = 0;aux < i-1; aux++)//laco para cada classificador treinar um objeto 
				
				
					label = one_objectClassifying(gTraining,out[0], randomico);/*esse label sempre retorna 0*/
					
					
					//vet_label[one_objectClassifying(gTraining,out[aux], randomico)-1] ++;//incrementa a posicao do label que foi classificado		
					printf("LABEL = %d\n",label);
				/*	
				///combina a classificacao
				maior = 0;//no vetor escolhe o indice 1 como maior
				for (l = 1; l < g->nlabels;l++){//percorre o vetor procurando o mais votado
					if (vet_label[maior] < vet_label[j])
						maior = j;//guarda apenas o j	//guarda//guarda o mais votado (+1 pois as classes comecam com 1)
				}
				label = maior+1;
				printf("inserir com probabilidade!!\n");
			
				if(g->node[randomico].truelabel == label){//se nao errou, insere com probabilidade
						//escolhe um numero aleatorio entre 0 e 1
						R = rand()/((double)RAND_MAX + 1);
						//calculando a o erro -= e_{i-1} / ( 1- e_{i-1} )
						erro = error[i-1]/(1-error[i-1]);
						
						if(erro < R){//se o erro estiver dentro desse valor sorteado
							for (n = 0; n < g->nfeats; n++)//percorre todos os atributos
								out[i]->node[randomico].feat[n] = g->node[j].feat[n];//copiando para o novo grafo os atributos
							out[i]->node[randomico].truelabel = g->node[j].truelabel;//copia o rotulo verdadeiro(supervisionado)
							out[i]->node[randomico].position = g->node[i].position;//copia a posicao no grafo original
							out[i]->node[randomico].relevant = 1;				
						}
			    */}
			    /*
				else{//se errou, insere de qualquer forma
					for (n = 0; n < g->nfeats; n++)//percorre todos os atributos
						out[i]->node[randomico].feat[n] = g->node[j].feat[n];//copiando para o novo grafo os atributos
					out[i]->node[randomico].truelabel = g->node[j].truelabel;//copia o rotulo verdadeiro(supervisionado)
					out[i]->node[randomico].position = g->node[i].position;//copia a posicao no grafo original
					out[i]->node[randomico].relevant = 1;				
				}
				
			}*/
			
			/*else//procura outro que ainda nao foi usado
				j--;*/
			
		//}
		/*
		//treinar o novo conjunto que foi formado
		opf_OPFTraining(out[i]);
	
		
		strcpy(fileName,"Classifier");
		sprintf(count, "%d", i);
		strcat(fileName,count);
		strcat(fileName,".opf");
		 
		WriteSubgraph(out[i],fileName);//grava o grafo no disco
		
		//calcular o erro	
		opf_OPFClassifying(out[i],gEvaluating);//classifica o novo conjunto de dados	
		error[i] = opf_error(gEvaluating);	
			printf("Probabilidade %f",error[i]);*/
		
	//}
	for (i = 0; i < gTraining->nnodes; i++)
		gTraining->node[i].relevant = 0;

	return out;
}
