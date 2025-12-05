#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#define TAM_LISTA 10000

// A lista precisa ser acessível, mas no fork, a memória é copiada.
// O filho herda uma cópia da lista preenchida pelo pai.
int lista[TAM_LISTA];

int cmpfunc(const void * a, const void * b) { return ( *(int*)a - *(int*)b ); }

double pegar_tempo() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

int main() {
    int pipe_media[2], pipe_mediana[2], pipe_desvio[2];
    pid_t p1, p2, p3;
    double resultado_media, resultado_mediana, resultado_desvio;
    double inicio, fim;

    // Preencher lista
    srand(time(NULL));
    for(int i = 0; i < TAM_LISTA; i++) lista[i] = rand() % 101;

    // Criar Pipes (Comunicação entre o Pai-Filho)
    if (pipe(pipe_media) < 0 || pipe(pipe_mediana) < 0 || pipe(pipe_desvio) < 0) {
        perror("Erro pipe"); exit(1);
    }

    printf(">> Executando com 3 PROCESSOS...\n");
    inicio = pegar_tempo();

    // --- Processo 1: Média ---
    p1 = fork();
    if (p1 == 0) {
        close(pipe_media[0]); // Fecha leitura
        double soma = 0;
        for(int i = 0; i < TAM_LISTA; i++) soma += lista[i];
        double res = soma / TAM_LISTA;
        write(pipe_media[1], &res, sizeof(double));
        close(pipe_media[1]);
        exit(0);
    }

    // --- Processo 2: Mediana ---
    p2 = fork();
    if (p2 == 0) {
        close(pipe_mediana[0]);
        // Como é um processo separado, pode ordenar 'lista' diretamente
        // pois é uma cópia da memória do pai. Não afeta os outros.
        qsort(lista, TAM_LISTA, sizeof(int), cmpfunc);
        double res;
        if(TAM_LISTA % 2 == 0) res = (lista[TAM_LISTA/2 - 1] + lista[TAM_LISTA/2]) / 2.0;
        else res = lista[TAM_LISTA/2];
        
        write(pipe_mediana[1], &res, sizeof(double));
        close(pipe_mediana[1]);
        exit(0);
    }

    // --- Processo 3: Desvio Padrão ---
    p3 = fork();
    if (p3 == 0) {
        close(pipe_desvio[0]);
        double soma = 0, soma_sq = 0, media_local;
        for(int i = 0; i < TAM_LISTA; i++) soma += lista[i];
        media_local = soma / TAM_LISTA;
        for(int i = 0; i < TAM_LISTA; i++) soma_sq += pow(lista[i] - media_local, 2);
        double res = sqrt(soma_sq / TAM_LISTA);
        
        write(pipe_desvio[1], &res, sizeof(double));
        close(pipe_desvio[1]);
        exit(0);
    }

    // --- Processo Pai ---
    // Fecha pontas de escrita que não usará
    close(pipe_media[1]); close(pipe_mediana[1]); close(pipe_desvio[1]);

    // Espera os filhos terminarem
    wait(NULL); wait(NULL); wait(NULL);

    // Lê resultados
    read(pipe_media[0], &resultado_media, sizeof(double));
    read(pipe_mediana[0], &resultado_mediana, sizeof(double));
    read(pipe_desvio[0], &resultado_desvio, sizeof(double));
    
    fim = pegar_tempo();

    printf("Média: %.2f | Mediana: %.2f | Desvio Padrão: %.2f\n", resultado_media, resultado_mediana, resultado_desvio);
    printf("Tempo TOTAL (Processos): %.6f segundos\n", fim - inicio);

    return 0;
}