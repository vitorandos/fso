#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUFFER 256
#define READ 0
#define WRITE 1

void calculate_time(struct timeval start, struct timeval end, int times[]){
  gettimeofday(&end, NULL);
  if (end.tv_usec >= start.tv_usec){
    times[0] = (int) end.tv_sec - (int) start.tv_sec; //seconds
    times[1] = (int) end.tv_usec - (int) start.tv_usec; //miliseconds
  }
  else {
    times[0] = (int) end.tv_sec - 1 - (int) start.tv_sec; //seconds
    times[1] = (int) end.tv_usec + 60 + (int) start.tv_usec; //miliseconds
  }
}

int get_largest_file_descriptor(int pipe_child1[], int pipe_child2[]){
  int largest;

  largest = pipe_child1[0];

  if (pipe_child1[1] > largest)
    largest = pipe_child1[1];
  if (pipe_child2[0] > largest)
    largest = pipe_child2[0];
  if (pipe_child2[1] > largest)
    largest = pipe_child2[1];

  return largest;
}

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

double calculate_current_time(struct timeval begin){
    struct timeval end;

    gettimeofday(&end, NULL);

    double current_time = end.tv_sec - begin.tv_sec;

    return current_time;
}

int main(void)
{
    int pipe_child1[2]; /* File descriptors pro pipe do filho preguiçoso*/
    int pipe_child2[2]; /* File descriptors pro pipe do filho ativo*/
    int times[2];
    pid_t child1, child2; /* Variável para armazenar o pid*/
    int message_number_child1 = 0;
    int message_number_child2 = 0;
    struct timeval start, end;
    char sleeper_child_message[BUFFER], user_message[BUFFER];
    int seconds, miliseconds;
    int largest_file_descriptor;
    fd_set read_set;
    fd_set write_set, error;
    int status;

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
  //      close(pipe_child2[READ]);
//        close(pipe_child2[WRITE]);

        //salvar string com tempo na string srt

        calculate_time(start, end, times);

        message_number_child1++;
        sprintf(sleeper_child_message, "0:%02d.%03d: Mensagem %02d do filho dorminhoco", times[0], times[1], message_number_child1);

        /* Escrevendo a string no pipe */
        write(pipe_child1[WRITE], sleeper_child_message, sizeof(sleeper_child_message) + 1);
        printf("wwhat\n");
        sleep(5);
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
        /*Como o child2 irá ESCREVER, fecha-se a LEITURA do Pipe neste lado*/
        close(pipe_child2[READ]);

        message_number_child2++;

        calculate_time(start, end, times);

        char final_message[BUFFER];
        sprintf(final_message, "0:%02d.%03d: Mensagem %02d do usuario <", times[0], times[1], message_number_child2);

        printf("Insira a mensagem a qual deseja enviar.\n");
        scanf("%s", user_message);

        strcat(user_message, ">");
        strcat(final_message, user_message);

        printf("DEUS %s\n", final_message);

        if (write(pipe_child2[WRITE], final_message, strlen(final_message) + 1) != strlen(final_message) + 1){
            printf("Error writing to the pipe.");
            _exit(EXIT_FAILURE);
        }
    }
    int result;
    printf("End of child processes\n");
    while(calculate_current_time(start) < 10){
        largest_file_descriptor = get_largest_file_descriptor(pipe_child1, pipe_child2);
        FD_ZERO(&read_set);
        FD_SET(pipe_child1[READ], &read_set);
        FD_SET(pipe_child2[READ], &read_set);

        /*
            SELECT NAO TA FUNFANDO DIREITO
            praticamente todas as vezes tá retornando -1, e -1 quer dizer que deu erro
            mais embaixo eu to imprimindo o erro e tá falando que o erro tá no file descriptor
            ou seja no primeiro argumento do select, mas não to sabendo o que colocar nele
        */
        result = select(largest_file_descriptor, &read_set, NULL, NULL, NULL);
        /* Processo Pai*/
        char str_recebida[BUFFER], str2[BUFFER];

        /* No filho, vamos ler. Então vamos fechar a entrada de ESCRITA do pipe */
        close(pipe_child1[WRITE]);
        close(pipe_child2[WRITE]);
        printf("RESULT %d\n", result);
        /* Lendo o que foi escrito no pipe, e armazenando isso em 'str_recebida' */
        if (result != -1){
        if (FD_ISSET(pipe_child1[READ], &read_set)){
          read(pipe_child1[READ], str_recebida, sizeof(str_recebida));
          calculate_time(start, end, times);
          printf("0:%02d.%03d:%s    FILHO DORMINHOCO\n\n", times[0], times[1], str_recebida);
          close(pipe_child1[READ]);
        }
        if (FD_ISSET(pipe_child2[READ], &read_set)){
          read(pipe_child2[READ], str2, sizeof(str2));
          calculate_time(start, end, times);
          printf("0:%02d.%03d:%s    FILHO ATIVO\n\n", times[0], times[1], str2);
          close(pipe_child2[READ]);
        }
        }
        // aqui imprime o erro do select
        if (result == -1){
          perror("select()");
    //return 0;
        }


    }

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
