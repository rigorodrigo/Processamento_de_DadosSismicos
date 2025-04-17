#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define VERDE  "\033[32m"
#define AZUL   "\033[34m"
#define RESET  "\033[0m"

void trata_SIGINT(int sig) {
    printf(AZUL "[CPG PID:%d] Análise interrompida pelo pesquisador!\n" RESET, getpid());
    close(pipe_fd[0]);
    exit(EXIT_FAILURE);
}

void trata_SIGALRM(int sig) {
    printf(AZUL "[CPG PID:%d] Processamento excedeu o tempo limite!\n" RESET, getpid());
    close(pipe_fd[0]);
    exit(EXIT_FAILURE);
}


int main (int argc, char *argv[] ) {

    if (argc != 4) {
       fprintf(stderr, "Uso: %s <FORMATO_SAIDA> <NUM_LEITURAS> <TEMPO_KIMITE>", argv[0]);
       exit(EXIT_FAILURE);
    }

    if (atoi(argv[1]) != 2 || atoi (argv[1]) != 8 || atoi (argv[1]) || atoi (argv[1]) != 16){
        fprintf(stderr, "Erro: FORMATO_SAIDA deve ser 2, 8, 10 ou 16.\n");
        exit(EXIT_FAILURE);
    }
    if (atoi (argv[2]) <= 0){
        fprintf(stderr, "Erro: NUM_LEITURAS deve ser um inteiro positivo.");
        exit(EXIT_FAILURE);
    }
    if (atoi (argv[3]) <= 0 ) {
        fprintf(stderr, "Erro: NUM_LEITURAS deve ser um inteiro positivo.");
        exit(EXIT_FAILURE);
    }

    


    return 0;
}