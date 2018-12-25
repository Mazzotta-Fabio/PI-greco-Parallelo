# PI-greco-Parallelo

Il progetto è stato sviluppato per l'esame di Programmazione Concorrente, Parallela e su Cloud per l'anno accademico 2017/2018 del corso di
Laurea Magistrale in Informatica dell'Università di Salerno.

### Progetto Assegnato: PIGreco using collective and value it on m4.large. Il progetto è stato sviluppato con MPI (Message Passing Interface) usando il linguaggio C.

### Esecuzione 
Per compilare i programmi con entrambi i metodi inserire il comando: `mpicc NOMEFILEC -o NOMEESEGUIBILE `
Per lanciare i programmi con entrambi i  metodi digitare il comando:  `mpirun -np NUMEROPROCESSORI -host IPMASTER,IPSLAVE NOMEESEGUBILE -size TAGLIAINPUT `
#### N.B. In entrambi i programmi è necessario inserire obbligatoriamente la taglia dell'input da linea di comando.
___
#### Descrizione Metodo Montecarlo:
Il processo Master alloca un array di puntatori di interi in maniera dinamica e poi chiama la funzione **riempiArray**. 
In questa funzione si  divide la quantità dei punti totali tra gli n processi e ne si calcola il resto; in questo modo viene gestito il **problema del resto** consentendo di dividere la quantità di punti per qualsiasi numero di processi. 
Poi nella funzione vi sono due cicli for in cui entrambi inseriscono nell'array la quantità di punti da calcolare per ogni processo. L'unica differenza fra i due è che un ciclo for fa l'incremento di uno sulla quantità di punti da assegnare: questo incremento rappresenta un unità di resto che viene assegnata ai processi totali del resto.
Poi viene chiamata la funzione **MPI_Scatter** che assegna ad ogni processo la quantità di punti parziale usando l'array calcolato in precedenza. Poi abbiamo che ogni processo, usando la quantità di punti ricevuta, calcola i punti appartenenti alla circonferenza chiamando la funzione **regolaMontecarlo**. Infine si fa la somma totale di tutti i punti calcolati dai vari processi attraverso la funzione **MPI_Reduce**. Il processo Master calcola l'approssimazione del pigreco e libera la memoria allocata.
#### Descrizione Metodo del Trapezio:
Il processo Master alloca due array di puntatori di interi in maniera dinamica e poi chiama la funzione **riempiArray**.
In questa funzione si divide la quantità di partizioni totali tra gli n processi e ne si calcola il resto; in questo modo viene gestito il **problema del resto** consentendo di dividere le partizioni per qualsiasi numero di processi.
Nella funzione vi sono due cicli for. Entrambi inseriscono in un array il punto dove iniziare a calcolare le varie partizioni e nell'altro array il punto che indica dove terminare il calcolo delle partizioni; l'unica differenza tra i due cicli è che un ciclo fa l'incremento di uno sul punto in cui terminare il calcolo della partizione: questo rappresenta unità di resto e viene assegnata a tutti i processi totali del resto.
Poi viene chiamata due volte la funzione **MPI_Scatter**, dove la prima volta si assegna ad ogni processo il punto iniziale in cui calcolare l'area della partizione, la seconda volta si assegna ad ogni processo il punto finale in cui calcolare l'area dell'ultima partizione e si usano i rispettivi array calcolati prima. Poi abbiamo che ogni processo, usando i punti di inizio e di fine di calcolo delle partizioni, calcola le aree delle partizioni ricevute chiamando la funzione **regolaTrapezio**. Infine si fa la somma totale di tutte le aree delle partizioni calcolate dai vari processi attraverso la funzione **MPI_Reduce**. Il processo Master calcola l'area totale, che corrisponde all' approssimazione del pigreco, e libera la memoria allocata.
## Benchmark
Sono stati effettuati una serie di test utilizzando delle istanze di tipo **m4.large** (2 core) di Amazon Web Services con **StarCluster-Ubuntu_12.04-x86_64-hvm** - ami-52a0c53b. In entrambi i metodi il tempo di esecuzione è stato considerato il momento successivo alla divisione dei  vari dati da parte del processo Master. I tempi sono misurati in secondi ed è stata utilizzata la funzione `MPI_Wtime()` per prendere i tempi. Durante la scrittura del readme i tempi vengono riportati in microsecondi.
#### Strong Scaling del metodo del Trapezio.
I test di Strong Scaling del metodo del trapezio sono stati effettuati fissando come quantità totale delle partizioni 10.000.000.
Nella tabella si riportano i tempi:
| N°Processore|  Tempo in microsecondi |
| ------------------- | :---------------: |
| **2**	|67167 |
| **4**	|34747 |
| **6**	|23643 |
| **8**	|20254 |
| **10**	|16103|
| **12**	|12817 |
| **14**	|11503|
| **16**	|9436|
 In allegato il grafico dello strong scaling del metodo del trapezio. Dal grafico si può notare che il tempo di esecuzione diminuisce all'aumentare del numero dei processori. Quindi il programma risulta essere più veloce quando si aumentano il numero di processori.
#### Strong Scaling del metodo Montecarlo.
I test di Strong Scaling del metodo Montecarlo sono stati effettuati fissando come quantità totale dei punti 100.000.
Nella tabella si riportano i tempi:

| N°Processore|  Tempo in microsecondi|
| ------------------- | :---------------: |
| **2**	|2089 |
| **4**	|1934|
| **6**	|1623 |
| **8**	|4212|
| **10**	|2894 |
| **12**	|1857 |
| **14**	|2167 |
| **16**	|1615|
In allegato il grafico dello strong scaling del metodo Montecarlo. Dal grafico si può notare che si ha un aumento dei tempi quando si usano 8 e 14 processori. In questo caso si ha un carico di overhead in cui la comunicazione raggiunge valori molto alti da influenzare il tempo di esecuzione.
#### Weak scaling del metodo del trapezio
Per effettuare i test di tipo Weak Scaling del metodo del trapezio si è deciso di fissare la quantità partizioni totale uguale a 2.500.000*p dove p è il numero di processori attivi. Di seguito la tabella con i tempi:

| Quantità Partizioni        | N°Processori    | Tempo in microsecondi |
| ------------- |:-------------:| -----:|
| 5.000.000    | 2 | 33678|
| 10.000.000  | 4 | 34779|
| 15.000.000    | 6 | 36030|
| 20.000.000   | 8| 37324|
| 25.000.000    | 10 | 36480|
| 30.000.000    | 12 | 35202|
| 35.0000.000    | 14 | 35683|
| 40.000.000   | 16 | 34636|
In allegato il grafico dello weak scaling del metodo del trapezio. Il grafico mostra che fino all'utilizzo 8 processori il tempo aumenta. Poi da quando si usano 10 processori il tempo diminuisce. Probabilmente all'aumentare dei processori fino ad 8 si ha un aumento di overhead di comunicazione tra i processori. Dai 10 processori invece si ha un calo di overhead e si mantiene mediamente lo stesso intervallo di tempo.

#### Weak Scaling del metodo Montecarlo
Per effettuare i test di tipo Weak Scaling del metodo Montecarlo si è deciso di fissare la quantità di punti totale uguale a 25.000*p dove p è il numero di processori attivi. Di seguito la tabella con i tempi:

| Quantità Punti        | N°Processori    | Tempo in microsecondi|
| ------------- |:-------------:| -----:|
| 50.000    | 2 | 1059|
| 100.000  | 4 | 1939|
| 150.000    | 6 | 2115|
| 200.000  | 8| 4362|
| 250.000   | 10 | 3662|
| 300.000   | 12 | 2554|
| 350.000    | 14 | 2645|
| 400.000  | 16 | 2492|
In allegato il grafico della weak scaling del metodo montecarlo. Osservando il grafico notiamo che anche fino all'utilizzo 8 processori il tempo aumenta. Poi da quando si usano 10 processori il tempo diminuisce. Probabilmente all'aumentare dei processori fino ad 8 si ha un aumento di overhead di comunicazione tra i vari processori. Da 10 processori  a salire, invece, si ha un calo di overhead.

### Fattori di scalabilità

Il fattore di strong scalability è stato calcolato tramite la formula: `t1 / ( N * tN ) * 100%`
Il fattore di weak scalability è stato calcolato tramite la formula: `(t1/tN) * 100%`

Di seguito vi sono i fattori di scalabilità del metodo del trapezio:

| N°Processori        | Weak Scaling    | Strong Scaling|
| ------------- |:-------------:| -----:|
| **2**  | 1.80% | 0.097%|
| **4**   | 1.74%| 0.093%|
| **6**   | 1.68% | 0.091%|
| **8**   | 1.62% | 0.0804%|
| **10**  | 1.66%| 0.0809%|
| **12**  | 1.72%| 0.0847%|
| **14**  | 1.69% | 0.0809%|
| **16**  | 1.75%| 0.0863%|

Di seguito vi sono i fattori di scalabilità del metodo del metodo Montecarlo:

| N°Processori        | Weak Scaling    | Strong Scaling|
| ------------- |:-------------:| -----:|
| **2**  | 0.61% | 0.58%|
| **4**   | 0.33%| 0.31%
| **6**   | 0.25% | 0.25%|
| **8**   | 0.15% | 0.073%|
| **10**  | 0.17% | 0.085%|
| **12**  | 0.256% | 0.11%|
| **14**  | 0.247%| 0.08%|
| **16**  | 0.262% | 0.095%|
