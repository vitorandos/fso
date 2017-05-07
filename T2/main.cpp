#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

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

int main(void)
{
    int pipe_child1[2]; /* File descriptors pro pipe do filho preguiçoso*/
    int pipe_child2[2]; /* File descriptors pro pipe do filho ativo*/
    pid_t child1, child2; /* Variável para armazenar o pid*/
    
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
    if (child1 < 0)
    {
        perror("Error creating child1\n");
        exit(1);
    }
    else if(child1 == 0) {
        /*Como o child1 irá ESCREVER, fecha-se a LEITURA do Pipe neste lado*/
        close(pipe_child1[READ]);

        //salvar string com tempo na string srt

        /* Escrevendo a string no pipe */
        write(pipe_child1[WRITE], str, sizeof(str) + 1);
        exit(0);
    }
    
    printf("Creating process for child2\n");
    
    /* Criando o processo para o filho ativo*/
    child2 = fork();
    if (child2 < 0)
    {
        perror("Error creating child2\n");
        exit(1);
    }
    else if(child2 == 0){
        //Implementar aqui filho ativo
    }

    printf("End of child processes\n");

    /* Processo Pai*/
    //char str_recebida[BUFFER];

    /* No filho, vamos ler. Então vamos fechar a entrada de ESCRITA do pipe */
    //close(pipe_child1[WRITE]);

    /* Lendo o que foi escrito no pipe, e armazenando isso em 'str_recebida' */
    //read(pipe_child1[READ], str_recebida, sizeof(str_recebida));

    //printf("String lida pelo filho no Pipe : '%s'\n\n", str_recebida);
    exit(0);

    return(0);
}
