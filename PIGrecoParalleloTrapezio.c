/*
 ============================================================================
 Nome       : PIGrecoParallelo.c
 Autore     : Mazzotta Fabio
 Versione   :
 Copyright  :
 Descrizione : Calcolo Pi Greco con metodo del trapezio
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
double regolaTrapezio(int indiceIniziale,int indiceFinale);
void riempiArray(int in[],int fine[], int dim_array,int partizioni);
/*intervalli dell'integrale*/
#define a 1E-7
#define b 1E-14


int main(int argc, char* argv[]){
	int  my_rank; /* rank del processo */
	int p; /*numero totale di processi*/
	double t1,t2;//variabili per il calcolo del tempo
	int inizio,fine;/*indici dell'intervallo di scansione delle partizioni da inviare ad ogni processo */
	int *arrayValoriInizio,*arrayValoriFine;/*gli inidici degli intervalli sono inseriti in due array separati*/
	double parSum,parTot,appPiGreco;
	int n;//indica la quantità totale delle partizioni presenti nell'area dell'integrale
	
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &p);



	if(my_rank==0){
		n=atoi(argv[argc-1]);
		arrayValoriInizio=(int *)malloc(p *sizeof(int));
		arrayValoriFine=(int *)malloc(p *sizeof(int));
		riempiArray(arrayValoriInizio,arrayValoriFine,p,n);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	t1=MPI_Wtime();

	/*inviamo la quantità parziale di partizione ad ogni processo in modo collettivo*/
	MPI_Scatter(arrayValoriInizio,1,MPI_INT,&inizio,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Scatter(arrayValoriFine,1,MPI_INT,&fine,1,MPI_INT,0,MPI_COMM_WORLD);
	

	//calcoliamo l'area parziale delle partizioni ricevute
	parSum=regolaTrapezio(inizio,fine);
	

	//calcoliamo la somma totale di tutte le aree trovate da ogni processo
	MPI_Reduce(&parSum,&parTot,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	t2=MPI_Wtime();

	if(my_rank==0){
		//calcoliamo il risultato finale di tutta l'area
		appPiGreco=4*a*parTot;
		printf("Approssimazione PIGreco: %f\nIl tempo impiegato è di %f\n",appPiGreco,t2-t1);
		free(arrayValoriInizio);
		free(arrayValoriFine);
	}
	
	
	MPI_Finalize();
	return 0;
}
/*Funzione che calcola gli intervalli di partizione da inviare ad ogni processo.
 * questa funzione inserisce gli indici di inizio e di fine in due array separati*/
void riempiArray(int in[],int fine[],int dim_array,int partizioni){
	int i,resto,dimProc;
	int contaIndici=0;
	dimProc=partizioni/dim_array;
	resto=partizioni%dim_array;
	for(i=0;i<resto;i++){
		in[i]=contaIndici;
		contaIndici=contaIndici+dimProc+1;
		fine[i]=contaIndici;
	}
	for(i=resto;i<dim_array;i++){
		in[i]=contaIndici;
		contaIndici=contaIndici+dimProc;
		fine[i]=contaIndici;
	}
}
/*la funzione calcola l'area di ogni partzione dell'integrale
 * f(x)=4*[integrale tra 0e1]1/1+x^2*
 */
double regolaTrapezio(int indiceIniziale,int indiceFinale){
	int i;
	double result,x2;
	//calcoliamo ogni area di ogni partizione
	for(i=indiceIniziale;i<indiceFinale;i++){
	   x2=b*i*i;
	   result=result+1.0/(1.0+x2);
	}
	/*restituisce l'area totale di ogni partizione*/
	return result;
}
