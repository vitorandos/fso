#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

#define BUFFER 256
#define READ 0
#define WRITE 1

int generateRandomNumber(){

    int max_rand_part = (RAND_MAX - 1) / 3.0;
    int random_number = rand();

    while(random_number >= RAND_MAX){
        random_number = rand();
    }

    int random_result;
    if(random_number >=0 && random_number <= max_rand_part){
        random_result = 0;
    }
    else if(random_number > max_rand_part && random_number <= max_rand_part*2){
        random_result = 1;
    }
    else if(random_number > max_rand_part*2 && random_number <= max_rand_part*3){
        random_result = 2;
    }

    return random_result;
}

void SleepFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}

int main(void)
{
    int pipe_child1[2]; /* File descriptors pro pipe do filho preguiçoso*/
    int pipe_child2[2]; /* File descriptors pro pipe do filho ativo*/
    pid_t child1, child2; /* Variável para armazenar o pid*/
    int message_number = 0;
    struct timeval start, end;
    char sleeper_child_message[BUFFER];
    int seconds, miliseconds;

    gettimeofday(&start, NULL);

    printf("Creating pipe for child1\n");

    /* Criando os Pipes */
    if(pipe(pipe_child1) < 0) {
        perror("Error creating pipe for child1\n") ;
        return -1 ;
    }

    printf("Creating pipe for child2\n");

    if(pipe(pipe_child1) < 0) {
        perror("Error creating pipe for child2\n") ;
        return -1 ;
    }

    printf("Creating process for child1\n");

    /* Criando o processo para o filho preguiçoso*/
    child1 = fork();
    if (child1 < 0){
        perror("Error creating child1\n");
        exit(1);
    }
    else if(child1 == 0) {
        /*Como o child1 irá ESCREVER, fecha-se a LEITURA do Pipe neste lado*/
        close(pipe_child1[READ]);

        //salvar string com tempo na string srt
        sleep(generateRandomNumber());

        gettimeofday(&end, NULL);

        if (end.tv_usec >= start.tv_usec){
          seconds = (int) end.tv_sec - (int) start.tv_sec;
          miliseconds = (int) end.tv_usec - (int) start.tv_usec;
        } else {
          seconds = (int) end.tv_sec - 1 - (int) start.tv_sec;
          miliseconds = (int) end.tv_usec + 60 + (int) start.tv_usec;
        }

        message_number++;
        sprintf(sleeper_child_message, "0:%02d.%03d: Mensagem %02d do filho dorminhoco", seconds, miliseconds, message_number);
        printf("deus eh top %s\n", sleeper_child_message);


        /* Escrevendo a string no pipe */
        write(pipe_child1[WRITE], sleeper_child_message, sizeof(sleeper_child_message) + 1);
        exit(0);
    }

    printf("Creating process for child2\n");

    /* Criando o processo para o filho ativo*/
    child2 = fork();
    if (child2 < 0){
        perror("Error creating child2\n");
        exit(1);
    }
    else if(child2 == 0){
        //Implementar aqui filho ativo
    }

    printf("End of child processes\n");

    /* Processo Pai*/
    char str_recebida[BUFFER];

    /* No filho, vamos ler. Então vamos fechar a entrada de ESCRITA do pipe */
    close(pipe_child1[WRITE]);

    /* Lendo o que foi escrito no pipe, e armazenando isso em 'str_recebida' */
    read(pipe_child1[READ], str_recebida, sizeof(str_recebida));

    printf("String lida pelo filho no Pipe : '%s'\n\n", str_recebida);
    printf("Parent waiting for children completion...\n");
    if (wait(NULL) == -1){
        printf("Error waiting.\n");
        exit(EXIT_FAILURE);
    }

    if (wait(NULL) == -1){
        printf("Error waiting.\n");
        exit(EXIT_FAILURE);
    }

    exit(0);

    return(0);
}