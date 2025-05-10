# Simulador ESR-CPG

## Resumo do Projeto

Este projeto implementa um simulador simples de um sistema de aquisição e processamento de dados com dois processos que se comunicam por meio de um pipe. O projeto foi desenvolvido por Pedro Rafael Domenighi e Rodrigo Rigo como exercício didático para demonstrar conceitos de programação com processos em C, comunicação entre processos, manipulação de sinais e operações de entrada/saída.

## Componentes do Sistema

O sistema consiste em dois processos principais:

1. **ESR (Emissor de Sinais Randômicos)**: Gera dados aleatórios de 16 bits e os envia através de um pipe.
2. **CPG (Conversor e Processador Geral)**: Recebe os dados do ESR, processa e exibe as informações no formato especificado.

## Funcionalidades

- Geração aleatória de números de 16 bits
- Comunicação entre processos via pipes
- Conversão de dados para diferentes formatos (binário, octal, decimal, hexadecimal)
- Tratamento de sinais (SIGINT e SIGALRM)
- Registro de operações em arquivo de log
- Temporização de execução com limite configurável

## Requisitos

- Sistema operacional tipo Unix/Linux
- Compilador C (GCC recomendado)

## Compilação

Para compilar o programa, use o comando:

```bash
gcc -o esr_cpg processamento.c
```

## Uso

Execute o programa com os seguintes parâmetros:

```bash
./esr_cpg <FORMATO_SAIDA> <NUM_LEITURAS> <TEMPO_LIMITE>
```

Onde:
- **FORMATO_SAIDA**: Formato de saída dos dados (2 para binário, 8 para octal, 10 para decimal, 16 para hexadecimal)
- **NUM_LEITURAS**: Número de leituras/dados a serem gerados
- **TEMPO_LIMITE**: Tempo máximo de execução em segundos

### Exemplos de uso:

```bash
./esr_cpg 10 5 30  # Formato decimal, 5 leituras, 30 segundos de tempo limite
./esr_cpg 2 10 60  # Formato binário, 10 leituras, 60 segundos de tempo limite
./esr_cpg 16 3 15  # Formato hexadecimal, 3 leituras, 15 segundos de tempo limite
```

## Saída do Programa

O programa exibe informações com código de cores:
- **Verde**: Mensagens do processo ESR
- **Azul**: Mensagens do processo CPG

Além disso, todas as operações são registradas no arquivo `registro.log`.

## Comportamento do Sistema

1. O processo principal cria os dois processos filhos (ESR e CPG)
2. ESR gera valores aleatórios de 16 bits e os envia para o CPG
3. CPG recebe os valores e os exibe no formato especificado
4. O sistema termina quando todas as leituras são concluídas ou quando o tempo limite é atingido

## Sinais

O sistema trata dois sinais principais:
- **SIGINT**: Interrompe o processamento quando o usuário pressiona Ctrl+C
- **SIGALRM**: Finaliza o processamento quando o tempo limite é atingido

## Arquivos de Log

Todas as operações são registradas em um arquivo chamado `registro.log` com timestamp para facilitar a análise da execução do programa.

## Objetivo Didático

Este projeto ilustra diversos conceitos importantes em programação de sistemas, incluindo:
- Criação e gerenciamento de processos
- Comunicação entre processos usando pipes
- Tratamento de sinais em C
- Manipulação de bits e conversão entre bases numéricas
- Registro de eventos em arquivo (logging)
- Programação com timeouts