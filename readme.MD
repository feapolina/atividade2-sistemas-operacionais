# Projeto: Estatísticas com Processos e Threads

Este projeto demonstra o uso de **processos** e **threads** para calcular estatísticas básicas (média, mediana e desvio padrão) de uma lista de números aleatórios. Ele compara o desempenho entre as duas abordagens.

## Estrutura do Projeto

- **processos_stats.c**: Implementação usando processos. Cada estatística é calculada em um processo separado, com comunicação via pipes.
- **threads_stats.c**: Implementação usando threads. Cada estatística é calculada em uma thread separada.

## Como Executar

1. Compile os programas:
   ```bash
   gcc -o processos_stats processos_stats.c -lm
   gcc -o threads_stats threads_stats.c -lpthread -lm
   ```

2. Execute os binários:
   ```bash
   ./processos_stats
   ./threads_stats
   ```

## Resultados

- **processos_stats**: Mostra o tempo total para calcular as estatísticas usando processos.
- **threads_stats**: Mostra o tempo total para calcular as estatísticas usando threads e também sequencialmente.

## Requisitos

- GCC (compilador C)
- Sistema operacional compatível com POSIX (Linux, macOS, etc.)

## Observações

- O código utiliza uma lista de 10.000 números aleatórios entre 0 e 100.
- O objetivo é comparar o desempenho entre processos e threads.