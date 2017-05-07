// arquivo: prodcons.c
// descricao: Programa produtor-consumidor com mutex
// Utiliza a biblioteca pthreads.
// para compilar:  cc -o phtread pthread.c -lpthread

#include <stdio.h>       
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define  FALSE 0
#define  TRUE  1

// Declaracao das variaveis de condicao:
pthread_mutex_t  mutex;

//Buffer
#define  BUFFERVAZIO 0
#define  BUFFERCHEIO 1

static volatile int keepRunning = 1;

int buffer[50];
int estado = BUFFERVAZIO;

void intHandler(int dummy) {
    keepRunning = 0;
}

void *handleInput(void *arguments) {

   char user_message[1000];
   signal(SIGINT, intHandler);

   while (keepRunning) { 
    scanf("%[^\n]", user_message);
   }

   printf("[aviso]: Termino solicitado. Aguardando threads...\n");
   return (NULL);
}

void produtor(){
	int i=0;
    int aguardar;
    int countProd = 0;
    srand((unsigned int) time(NULL));

	printf("Inicio produtor\n");
	while (keepRunning){
        //produzir item
    	int randomNumber = rand(); 

        do{
            pthread_mutex_lock(&mutex);
        	aguardar = FALSE;
		    if (estado == BUFFERCHEIO){
				aguardar = TRUE;
		        pthread_mutex_unlock(&mutex);
			}
		} while (aguardar == TRUE);

    	//inserir item
        buffer[countProd] = randomNumber;
        printf("numero gerado: %d escrito em %d\n", randomNumber, countProd);
        if(countProd == 50)
    		estado = BUFFERCHEIO;
    
    	pthread_mutex_unlock(&mutex);
		countProd++;
		usleep(100);
    }
    
	printf("Produtor terminado \n"); 
}

void consumidor(int id){
	int item;
	int aguardar;
	int countCons = 0;

	printf("Inicio consumidor %d \n",id);
	while (keepRunning){
        // retirar item da fila
        do{
            pthread_mutex_lock(&mutex);
		    aguardar = FALSE;
		    if (estado == BUFFERVAZIO){
		        aguardar = TRUE;
				pthread_mutex_unlock(&mutex);
			}
        } while (aguardar == TRUE);
        
        printf("count = %d\n", countCons);
	    item = buffer[countCons];
	    if(countCons == 50){
	    	estado = BUFFERVAZIO;
	    	pthread_mutex_unlock(&mutex);
    	}

	    // processar item
        printf("Consumidor %d consumiu item %d\n", id, item); 
		countCons++;
 	    usleep(150);
    }
	printf("Consumidor %d terminado \n", id); 
}

int main()
{ 
	pthread_t control;
	pthread_t prod1;
	pthread_t cons1;
	pthread_t cons2;
	
	printf("Iniciando variaveis de sincronizacao.\n");
	pthread_mutex_init(&mutex,NULL);
	
	printf("Disparando thread controle\n");
	pthread_create(&control, NULL, handleInput, NULL);

    printf("Disparando thread produtora\n");
	pthread_create(&prod1, NULL, (void*) produtor,NULL);

    printf("Disparando threads consumidores\n");
	pthread_create(&cons1, NULL, (void*) consumidor,1);
	pthread_create(&cons2, NULL, (void*) consumidor,2);

	pthread_join(prod1,NULL);
	pthread_join(cons1,NULL);
	pthread_join(cons2,NULL);
	
    printf("Terminado processo Produtor-Consumidor.\n\n");
}
