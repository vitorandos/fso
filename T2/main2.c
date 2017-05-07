#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>

#define BUFFER 10000
#define READ 0
#define WRITE 1

static int* is_parent_running;

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

void calculate_time(int *time, struct timeval start, struct timeval end){ // x - y
  if (end.tv_usec < start.tv_usec){
    int nsec = (start.tv_usec - end.tv_usec) / 1000000 + 1;
    start.tv_usec -= 1000000 * nsec;
    start.tv_sec += nsec;
  }
  if (end.tv_usec - start.tv_usec > 1000000){
    int nsec = (end.tv_usec - start.tv_usec) / 1000000;
    start.tv_usec += 1000000 * nsec;
    start.tv_sec -= nsec;
  }
  time[0] = end.tv_sec - start.tv_sec; // seconds
  time[1] = (end.tv_usec - start.tv_usec) / 1000; // miliseconds
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
    int time[2];
    pid_t child1, child2; /* Variável para armazenar o pid*/
    int message_number_child1 = 0;
    int message_number_child2 = 0;
    struct timeval start, end;
    char sleeper_child_message[BUFFER], user_message[BUFFER];
    int seconds, miliseconds;
    fd_set read_set;
    fd_set write_set, error;
    is_parent_running = mmap(NULL, sizeof *is_parent_running, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *is_parent_running = 1;

    FILE *file = fopen("output.txt", "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    gettimeofday(&start, NULL);

    /* Criando os Pipes */
    if(pipe(pipe_child1) < 0) {
        perror("Error creating pipe for child1\n") ;
        return -1 ;
    }

    if(pipe(pipe_child2) < 0) {
        perror("Error creating pipe for child2\n") ;
        return -1 ;
    }

    /* Criando o processo para o filho preguiçoso*/
    child1 = fork();
    if (child1 < 0){
        perror("Error creating child1\n");
        exit(1);
    }
    else if(child1 > 0) {
        while(*is_parent_running){
            struct timeval end_child1;
            /*Como o child1 irá ESCREVER, fecha-se a LEITURA do Pipe neste lado*/
            close(pipe_child1[READ]);

            gettimeofday(&end_child1, NULL);

            calculate_time(time, start, end_child1);

            message_number_child1++;
            sprintf(sleeper_child_message, "0:%02d.%03d:\tMensagem %02d do filho dorminhoco", time[0], time[1], message_number_child1);
            /* Escrevendo a string no pipe */
            if (write(pipe_child1[WRITE], sleeper_child_message, strlen(sleeper_child_message) + 1) != strlen(sleeper_child_message) + 1){
              printf("Error writing to the pipe.");
              _exit(EXIT_FAILURE);
            }

            sleep(generateRandomNumber());
        }
    }

    /* Criando o processo para o filho ativo*/
    child2 = fork();
    if (child2 < 0){
        perror("Error creating child2\n");
        exit(1);
    }
    else if(child2 > 0){
        while(*is_parent_running){
        struct timeval end_child2;
            /*Como o child2 irá ESCREVER, fecha-se a LEITURA do Pipe neste lado*/
            close(pipe_child2[READ]);

            message_number_child2++;

            char end_of_final_message[BUFFER];
            char final_message[BUFFER];
            sprintf(end_of_final_message, "Mensagem %02d do filho ativo <", message_number_child2);

            fflush(stdin);
            __fpurge(stdin);
            scanf("%[^\n]", user_message);

            strcat(user_message, ">");
            strcat(end_of_final_message, user_message);

            gettimeofday(&end_child2, NULL);
            calculate_time(time, start, end_child2);

            sprintf(final_message, "0:%02d.%03d:\t", time[0], time[1]);

            strcat(final_message, end_of_final_message);

            if (write(pipe_child2[WRITE], final_message, strlen(final_message) + 1) != strlen(final_message) + 1){
                printf("Error writing to the pipe.");
                _exit(EXIT_FAILURE);
            }
        }
    }

    int result;

    int count = 0;
    while(calculate_current_time(start) < 30){
        char str_recebida_child1[BUFFER];
        char str_recebida_child2[BUFFER];
        int time_child1[2], time_child2[2];
        char final_string1[BUFFER], final_string2[BUFFER];

        FD_ZERO(&read_set);
        // a função read retorna - 1

        FD_SET(pipe_child1[READ], &read_set);
        FD_SET(pipe_child2[READ], &read_set);

        result = select(get_largest_file_descriptor(pipe_child1, pipe_child2)+1, &read_set, NULL, NULL, NULL);

        /* Processo Pai*/

        if (result != -1){
            if (FD_ISSET(pipe_child1[READ], &read_set)){
                close(pipe_child1[WRITE]);

                /* Lendo o que foi escrito no pipe, e armazenando isso em 'str_recebida' */

                if(read(pipe_child1[READ], str_recebida_child1, sizeof(str_recebida_child1)) != -1 ){
                  gettimeofday(&end, NULL);
                  calculate_time(time_child1, start, end);
                  sprintf(final_string1, "0:%02d.%03d:\t", time_child1[0], time_child1[1]);
                  strcat(final_string1, str_recebida_child1);
                  fprintf(file, "%s\n", final_string1);
                }

            }
            if (FD_ISSET(pipe_child2[READ], &read_set)){
                close(pipe_child2[WRITE]);
                /* Lendo o que foi escrito no pipe, e armazenando isso em 'str_recebida' */
                if (read(pipe_child2[READ], str_recebida_child2, sizeof(str_recebida_child2))){
                  gettimeofday(&end, NULL);
                  calculate_time(time_child2, start, end);
                  sprintf(final_string2, "0:%02d.%03d:\t", time_child2[0], time_child2[1]);
                  strcat(final_string2, str_recebida_child2);
                  fprintf(file, "%s\n", final_string2);
                }
            }
        }
        if (result == -1)
            perror("select()");
    }

    fclose(file);

    *is_parent_running=0;

    wait(NULL);

    return(0);
}
