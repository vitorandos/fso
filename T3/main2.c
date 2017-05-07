// arquivo: prodcons.c
// descricao: Programa produtor-consumidor com mutex
// Utiliza a biblioteca pthreads.
// para compilar:  cc -o phtread pthread.c -lpthread

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define  FALSE 0
#define  TRUE  1

// Declaracao das variaveis de condicao:
pthread_mutex_t  mutex;

//Buffer
#define  BUFFERVAZIO 0
#define  BUFFERCHEIO 1
#define  BUFFERCOMITENS 2

static volatile int keepRunning = 1;

int buffer[50];
int buffer_position = -1;
int estado = BUFFERVAZIO;
FILE *file;

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
    buffer_position++;
    buffer[buffer_position] = randomNumber;
    printf("numero gerado: %d escrito em %d\n", randomNumber, buffer_position);
    fprintf(file, "[producao] Numero gerado: %d\n", randomNumber);
    if (buffer_position > 0 && buffer_position < 50)
      estado = BUFFERCOMITENS;
    else if(buffer_position == 50)
      estado = BUFFERCHEIO;

    pthread_mutex_unlock(&mutex);
		usleep(100000);
    }

	printf("Produtor terminado \n");
  pthread_exit(0);
}

void consumidor(char *id){
	int item;
	int aguardar;

	printf("Inicio consumidor %c \n", *id);
	while (keepRunning){
    // retirar item da fila
    do {
      pthread_mutex_lock(&mutex);
		  aguardar = FALSE;
		  if (estado == BUFFERVAZIO){
		    aguardar = TRUE;
			  pthread_mutex_unlock(&mutex);
		  }
    } while (aguardar == TRUE);

    printf("count = %d\n", buffer_position);
	  item = buffer[buffer_position];
	  if (buffer_position == 0){
	    estado = BUFFERVAZIO;
	    pthread_mutex_unlock(&mutex);
    }

	  // processar item
    printf("Consumidor %c consumiu item %d\n", *id, item);
    fprintf(file, "[consumo %c]: Numero lido: %d\n", *id, item);
		buffer_position--;
    printf("buffer %d\n", buffer_position);
    pthread_mutex_unlock(&mutex);
 	  usleep(150000);
  }
	printf("Consumidor %c terminado \n", *id);
  pthread_exit(0);
}

int main(int argc, char const *argv[]){
	pthread_t control;
	pthread_t prod1;
	pthread_t cons1;
	pthread_t cons2;
  char id_consumidor1 = 'a';
  char id_consumidor2 = 'b';
  char file_name[100] = "";

  if (argc > 1){
    strcat(file_name, argv[1]);
    for (int arg_position = 2; argv[arg_position] != '\0'; arg_position++){
      strcat(file_name, " ");
      strcat(file_name, argv[arg_position]);
    }
    strcat(file_name, ".txt");
  } else {
    strcat(file_name, "output.txt");
    printf("Nenhum nome de arquivo especifico foi dado, será criado um arquivo com o nome padrão \"%s\"\n", file_name);
  }

  file = fopen(file_name, "w");
  if (file == NULL)
  {
      printf("Erro ao abrir arquivo!\n");
      exit(1);
  }

	printf("Iniciando variaveis de sincronizacao.\n");
	pthread_mutex_init(&mutex,NULL);

	printf("Disparando thread controle\n");
	pthread_create(&control, NULL, handleInput, NULL);

  printf("Disparando thread produtora\n");
	pthread_create(&prod1, NULL, (void*) produtor,NULL);

  printf("Disparando threads consumidores\n");
	pthread_create(&cons1, NULL, (void*) consumidor, &id_consumidor1);
	pthread_create(&cons2, NULL, (void*) consumidor, &id_consumidor2);

	pthread_join(prod1,NULL);
	pthread_join(cons1,NULL);
	pthread_join(cons2,NULL);

  printf("Terminado processo Produtor-Consumidor.\n\n");
}
