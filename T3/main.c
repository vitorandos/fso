#include <stdio.h>       
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define NUM_THREADS 2

static volatile int keepRunning = 1;
int buffer[50];
int count;

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

void *producesNumbers(void *arguments) {
   srand((unsigned int) time(NULL));   // should only be called once
   while(keepRunning){
       int randomNumber = rand(); 
       buffer[count] = randomNumber;
       
       usleep(100);
       count++;
       printf("[producao]: Numero gerado: %d\n", randomNumber);
   }
}

int main (){

  pthread_t threads[NUM_THREADS];
  int i;
  
  count = 0;
  
  pthread_create(&threads[1], NULL, handleInput, NULL);
  pthread_create(&threads[2], NULL, producesNumbers, NULL);

  printf("Thread principal a esperar a terminação das threads criadas\n");

  for(i=0;i<NUM_THREADS;i++)
      pthread_join(threads[i],NULL);  /* Esperara a junção das threads */ 
      
   printf("Fim das threads\n");

  return 0;   /* O programa chegará aqui. */ 

}
