#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

int formato_saida;
int num_leituras;
int tempo_limite;
int pipe_fd[2];
pid_t pid_esr, pid_cpg;

#define VERDE  "\033[32m"
#define AZUL   "\033[34m"
#define RESET  "\033[0m"

void trata_SIGINT(int sig) {
    printf(AZUL "[CPG PID:%d] Análise interrompida pelo pesquisador !\n" RESET, getpid());
    close(pipe_fd[0]);
    exit(EXIT_SUCCESS);
}

void trata_SIGALRM(int sig) {
    printf(AZUL "[CPG PID:%d] Processamento excedeu o tempo limite!\n" RESET, getpid());
    close(pipe_fd[0]);
    exit(EXIT_SUCCESS);
}

void print_num_convertido(unsigned short valor) {
    switch (formato_saida) {
        case 2:
            printf("Formato 2: ");
            for (int i = 15; i >= 0; i--) {
                printf("%d", (valor >> i) & 1);
            }
            break;
        case 8:
            printf("Formato 8: %o", valor);
            break;
        case 10:
            printf("Formato 10: %u", valor);
            break;
        case 16:
            printf("Formato 16: %X", valor);
            break;
    }
}

void print_bin(unsigned short valor) {
    for (int i = 15; i >= 0; i--) {
        printf("%d", (valor >> i) & 1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <FORMATO_SAIDA> <NUM_LEITURAS> <TEMPO_LIMITE>\n", argv[0]);
        return 1;
    }

    formato_saida = atoi(argv[1]);
    num_leituras = atoi(argv[2]);
    tempo_limite = atoi(argv[3]);

    if (formato_saida != 2 && formato_saida != 8 && formato_saida != 10 && formato_saida != 16) {
        fprintf(stderr, "Erro: FORMATO_SAIDA deve ser 2, 8, 10 ou 16.\n");
        return 1;
    }

    if (num_leituras <= 0 || tempo_limite <= 0) {
        fprintf(stderr, "Erro: NUM_LEITURAS e TEMPO_LIMITE devem ser inteiros positivos.\n");
        return 1;
    }
//pipe
    if (pipe(pipe_fd) == -1) {
        perror("Erro ao criar pipe!");
        return 1;
    }

    pid_esr = fork();

    if (pid_esr < 0) {
        perror("Erro ao criar processo filho!");
        return 1;
    }

    if (pid_esr == 0) {
        //ESR
        close(pipe_fd[0]); 
        srand(time(NULL) ^ getpid());

        for (int i = 0; i < num_leituras; i++) {
            unsigned short dado = rand() % 65536;
            write(pipe_fd[1], &dado, sizeof(unsigned short));

            printf(VERDE "[ESR PID:%d] Leitura #%d: ", getpid(), i + 1);
            print_bin(dado);
            printf(" transmitida.\n" RESET);

            sleep(1);
        }

        close(pipe_fd[1]);
        exit(0);

    } else {
        //CPG
        pid_cpg = getpid();
        close(pipe_fd[1]); // Fecha escrita

        signal(SIGINT, trata_SIGINT);
        signal(SIGALRM, trata_SIGALRM);
        alarm(tempo_limite);

        unsigned short valor;
        
        int leituras_feitas = 0;

        while (leituras_feitas < num_leituras && read(pipe_fd[0], &valor, sizeof(unsigned short)) > 0) {
            printf(AZUL "[CPG PID:%d] Recebido: ", pid_cpg);
            print_bin(valor);
            printf(" -> ");
            print_num_convertido(valor);
            printf("\n" RESET);

            leituras_feitas++;
        }

        close(pipe_fd[0]);
        wait(NULL);
        printf(AZUL "[CPG PID:%d] Conclusão Normal.\n" RESET, pid_cpg);
    }

    return 0;
}
