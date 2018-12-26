/*
 ============================================================================
 Nome        : PIGrecoParalleloMontecarlo.c
 Autore      : Mazzotta Fabio
 Version     :
 Copyright   :
 Descrizione : Calcolo Pi Greco con il metodo MonteCarlo
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"
int regolaMontecarlo(int puntiTotQuadrato);
void riempiArray(int array[],int dim_array,int partizioni);

int main(int argc, char* argv[]){
	int  my_rank; /* rank del processo */
	int p; /*numero totale di processi*/
	double t1,t2;//variabili per il calcolo del tempo
	int puntiTotQuad;//indica il numero di punti totali nel quadrato
	int recvBuf=0;//indica la quantità di punti da calcolare di ogni processo
	int *arrayValori;
	double pigreco;
	int puntiCircPar,puntiCircTot;
	

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if(my_rank==0){
		puntiTotQuad=atoi(argv[argc-1]);
		arrayValori=(int *)malloc(p *sizeof(int));
		riempiArray(arrayValori,p,puntiTotQuad);
	}

	/*inviamo la quantità dei punti ad ogni processo in maniera collettiva*/
	MPI_Scatter(arrayValori,1,MPI_INT,&recvBuf,1,MPI_INT,0,MPI_COMM_WORLD);
	
	//prendiamo i tempi
	MPI_Barrier(MPI_COMM_WORLD);
	t1=MPI_Wtime();

	//calcoliamo i punti parziali della circonferenza ricevuti
	puntiCircPar=regolaMontecarlo(recvBuf);
	

	//calcoliamo i punti totali della circonferenza trovati da ogni processo
	MPI_Reduce(&puntiCircPar,&puntiCircTot,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	t2=MPI_Wtime();

	if(my_rank==0){
		//calcoliamo il pigreco
		pigreco=(double)puntiCircTot/puntiTotQuad*4;
		printf("Approssimazione PIGreco: %f\nIl tempo impiegato è di %f\n",pigreco,t2-t1);
		free(arrayValori);
	}
	MPI_Finalize();
	return 0;
}
/* funzione che calcola la quantità di punti parziale da inviare per ogni processo*/
void riempiArray(int array[],int dim_array,int punti_quad){
	int i,resto,dimProc;
	dimProc=punti_quad/dim_array;
	resto=punti_quad%dim_array;
	for(i=0;i<resto;i++){
		array[i]=dimProc+1;
	}
	for(i=resto;i<dim_array;i++){
		array[i]=dimProc;
	}
}
/*funzione che restituisce il numero di punti appartenenti alla circonferenza*/
int regolaMontecarlo(int puntiTotQuadrato){
	srand(time(NULL));
	double x,y;//coordinate dei punti che sono comprese tra 0 e 1
	int puntiTotCirc=0;//punti totali nella circonferenza
	int i;
	double z;
	for(i=0;i<puntiTotQuadrato;i++){
		x=(double)rand()/RAND_MAX;
	    y=(double)rand()/RAND_MAX;
	    z=x*x+y*y;
	    if(z<=1){//verifichiamo se il punto appartiene alla circonferenza oppure no
	        puntiTotCirc++;
	    }
	}
	//restituiamo i punti appartenenti alla circonferenza che ci servono per calcolare il pi greco
	return puntiTotCirc;
}
