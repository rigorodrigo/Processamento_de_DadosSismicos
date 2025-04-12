#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>


int main (int argc, char *argv[] ) {

    if (argc != 4) {
       fprintf(stderr, "Uso: %s <FORMATO_SAIDA> <NUM_LEITURAS> <TEMPO_KIMITE>", argv[0]);
       exit(EXIT_FAILURE);
    }

    


    return 0;
}