// Componentes : Pedro Rafael Domenighi e Rodrigo Rigo

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define VERDE  "\033[32m"
#define AZUL   "\033[34m"
#define RESET  "\033[0m"

int pipefd [2], formato_saida,num_leituras,tempo_limite;
pid_t pid_esr, pid_cpg;

// implementação do log
void escreve_log(const char *mensagem) {
    FILE *log = fopen("registro.log", "a");
    if (log) {
        time_t agora = time(NULL);
        fprintf(log, "[%ld] %s\n", agora, mensagem);
        fclose(log);
    }
}

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
// Função ESR
void processo_esr() {
    close(pipefd[0]); // Fecha leitura
    srand(time(NULL) ^ getpid());

    for (int i = 0; i < num_leituras; i++) {
        unsigned short dado = rand() % 65536;
        write(pipefd[1], &dado, sizeof(unsigned short));

        printf(VERDE "[ESR PID:%d] Leitura #%d: ", getpid(), i + 1);
        printa_numero_binario(dado);
        printf(" transmitida.\n" RESET);
        fflush(stdout);

        char log_msg[100];
        sprintf(log_msg, "ESR PID:%d - Leitura #%d transmitida: %u", getpid(), i + 1, dado);
        escreve_log(log_msg);

        sleep(1);
    }

    escreve_log("ESR finalizou transmissão.");
    close(pipefd[1]);
    exit(EXIT_SUCCESS);
}
//Função CPG
void processo_cpg() {
    close(pipefd[1]); // Fecha escrita
    signal(SIGINT, trata_SIGINT);
    signal(SIGALRM, trata_SIGALRM);
    alarm(tempo_limite);

    unsigned short valor;
    int leitura = 1;

    while (read(pipefd[0], &valor, sizeof(unsigned short)) > 0) {
        printf(AZUL "[CPG PID:%d] Recebido: ", getpid());
        printa_numero_binario(valor);
        printf(" -> ");
        printa_numero_convertido(valor);
        fflush(stdout);

        char log_msg[100];
        sprintf(log_msg, "CPG PID:%d - Leitura #%d recebida: %u", getpid(), leitura, valor);
        escreve_log(log_msg);
        leitura++;
    }

 

    escreve_log("CPG finalizou processamento.");
    close(pipefd[0]);
    printf(AZUL "[CPG PID:%d] Conclusão normal.\n" RESET, getpid());
    exit(EXIT_SUCCESS);
}



int main (int argc, char *argv[] ) {

    if (argc != 4) {
       fprintf(stderr, "Uso: %s <FORMATO_SAIDA> <NUM_LEITURAS> <TEMPO_LIMITE>\n", argv[0]);
       exit(EXIT_FAILURE);
    }

    formato_saida = atoi(argv[1]);
    num_leituras = atoi(argv[2]);
    tempo_limite = atoi(argv[3]);

    if (formato_saida != 2 && formato_saida != 8 && formato_saida != 10 && formato_saida != 16){
        fprintf(stderr, "Erro: FORMATO_SAIDA deve ser 2, 8, 10 ou 16.\n");
        exit(EXIT_FAILURE);
    }
    if (num_leituras <= 0){
        fprintf(stderr, "Erro: NUM_LEITURAS deve ser um inteiro positivo.\n");
        exit(EXIT_FAILURE);
    }
    if (tempo_limite <= 0 ) {
        fprintf(stderr, "Erro: NUM_LEITURAS deve ser um inteiro positivo.\n");
        exit(EXIT_FAILURE);
    }

    if (pipe (pipefd) == -1) {
        perror("erro no pipe");
        exit(EXIT_FAILURE);
    }
    
    escreve_log("Início da simulação.");
    
    pid_esr = fork();
    if (pid_esr < 0) {
        perror("Erro ao criar processo ESR");
        escreve_log("Erro ao criar processo ESR.");
        exit(EXIT_FAILURE);
    }

    if (pid_esr == 0) {
        processo_esr();
    }

    pid_cpg = fork();
    if (pid_cpg < 0) {
        perror("Erro ao criar processo CPG");
        escreve_log("Erro ao criar processo CPG.");
        exit(EXIT_FAILURE);
    }

    if (pid_cpg == 0) {
        processo_cpg();
    }

    close (pipefd[0]);
    close (pipefd[1]);
    char log_msg[100];
    sprintf(log_msg, "Processos criados: ESR PID=%d, CPG PID=%d", pid_esr, pid_cpg);
    escreve_log(log_msg);

    waitpid(pid_esr,NULL,0);
    waitpid(pid_cpg,NULL,0);

    escreve_log("Execução encerrada com sucesso.");
    return 0;
}
