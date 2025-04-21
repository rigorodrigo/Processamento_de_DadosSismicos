#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define VERDE  "\033[32m"
#define AZUL   "\033[34m"
#define RESET  "\033[0m"

int pipefd [2], formato_saida,num_leituras,tempo_limite;
pid_t pid_esr, pid_cpg;

void trata_SIGINT(int sig) {
    printf(AZUL "[CPG PID:%d] Análise interrompida pelo pesquisador!\n" RESET, getpid());
    close(pipefd[0]);
    exit(EXIT_FAILURE);
}

void trata_SIGALRM(int sig) {
    printf(AZUL "[CPG PID:%d] Processamento excedeu o tempo limite!\n" RESET, getpid());
    close(pipefd[0]);
    exit(EXIT_FAILURE);
}

void printa_numero_convertido (unsigned short n){
    switch (formato_saida)
    {
    case 2:
        printf("Formato 2: ");
        for (int i = 15; i >= 0 ; i --){
            printf("%d", (n >> i) & 1);
        }
        printf("\n");
        break;
    
    case 8: 
        printf("Formato 8: %o\n", n);
        break;

    case 10: 
        printf("Formato 10: %u\n", n);
        break;
    
    case 16: 
        printf("Formato 16: %X\n", n);
        break;

    default:
        fprintf(stderr, "Erro: FORMATO_SAIDA deve ser 2, 8, 10 ou 16.\n");  // não é necessário, mas para manter o default eu deixei
        exit(EXIT_FAILURE);
    }
}

void printa_numero_binario(unsigned short n){
    for (int i = 15; i >= 0 ; i --){
        printf("%d", (n >> i) & 1);
    }
}


int main (int argc, char *argv[] ) {

    formato_saida = atoi(argv[1]);
    num_leituras = atoi(argv[2]);
    tempo_limite = atoi(argv[3]);

    if (argc != 4) {
       fprintf(stderr, "Uso: %s <FORMATO_SAIDA> <NUM_LEITURAS> <TEMPO_LIMITE>", argv[0]);
       exit(EXIT_FAILURE);
    }

    if (formato_saida != 2 && formato_saida != 8 && formato_saida != 10 && formato_saida != 16){
        fprintf(stderr, "Erro: FORMATO_SAIDA deve ser 2, 8, 10 ou 16.\n");
        exit(EXIT_FAILURE);
    }
    if (num_leituras <= 0){
        fprintf(stderr, "Erro: NUM_LEITURAS deve ser um inteiro positivo.");
        exit(EXIT_FAILURE);
    }
    if (tempo_limite <= 0 ) {
        fprintf(stderr, "Erro: NUM_LEITURAS deve ser um inteiro positivo.");
        exit(EXIT_FAILURE);
    }

    if (pipe (pipefd) == -1) {
        perror("erro no pipe");
        exit(EXIT_FAILURE);
    }

    pid_esr = fork();

    if (pid_esr < 0) {
        perror ("erro ao criar processo filho");
        exit(EXIT_FAILURE);
    }

    if (pid_esr == 0) {       // processo ESR
        close(pipefd[0]);   // fechando a extremidade de leitura do pipe
        srand(time(NULL) ^ getpid());

        for (int i = 0; i < num_leituras; i++){
            unsigned short dado = rand() % 65536;            // gerando um num binário de 16 bits
            write (pipefd[1], &dado , sizeof(unsigned short));            // envia o dado para o pipe
           
            printf(VERDE "[ESR PID:%d] Leitura #%d: ", getpid(), i + 1);
            printa_numero_binario(dado);
            printf(" transmitida.\n" RESET);

            sleep(1);
        }

        close (pipefd[1]); // fecha a extremidade de escrita do pipe
        exit(0);
    }

    else {  // processo CPG

        pid_cpg = fork();   
        close(pipefd[1]); // Fecha escrita

        signal(trata_SIGINT, trata_SIGINT);
        signal(trata_SIGALRM, trata_SIGALRM);
        alarm(tempo_limite); 
    
        unsigned short valor;  // variável que receberá cada leitura do pipe 

    }

    return 0;
}