#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h> 

#define TAM_LISTA 10000

int lista[TAM_LISTA];
double media_global, mediana_global, desvio_global;

// Função de comparação para o quicksort
int cmpfunc(const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

// Retorna tempo atual em segundos (double)
double pegar_tempo() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

void preencher_lista() {
    srand(time(NULL));
    for(int i = 0; i < TAM_LISTA; i++) {
        lista[i] = rand() % 101; // 0 a 100
    }
}

// --- Funções de Cálculo---

void realizar_media() {
    double soma = 0;
    for(int i = 0; i < TAM_LISTA; i++) soma += lista[i];
    media_global = soma / TAM_LISTA;
}

void realizar_mediana() {
    // Copia o vetor para não ordenar o original 
    // enquanto outras threads o leem (tenta evitar Race Condition)
    int *lista_local = malloc(TAM_LISTA * sizeof(int));
    for(int i=0; i<TAM_LISTA; i++) lista_local[i] = lista[i];
    
    qsort(lista_local, TAM_LISTA, sizeof(int), cmpfunc);
    
    if(TAM_LISTA % 2 == 0)
        mediana_global = (lista_local[TAM_LISTA/2 - 1] + lista_local[TAM_LISTA/2]) / 2.0;
    else
        mediana_global = lista_local[TAM_LISTA/2];
    
    free(lista_local);
}

void realizar_desvio() {
    double soma = 0, soma_sq = 0, media_local;
    for(int i = 0; i < TAM_LISTA; i++) soma += lista[i];
    media_local = soma / TAM_LISTA;

    for(int i = 0; i < TAM_LISTA; i++) 
        soma_sq += pow(lista[i] - media_local, 2);
    
    desvio_global = sqrt(soma_sq / TAM_LISTA);
}

// --- Wrappers para Pthread ---
void *thread_media(void *arg) { realizar_media(); pthread_exit(NULL); }
void *thread_mediana(void *arg) { realizar_mediana(); pthread_exit(NULL); }
void *thread_desvio(void *arg) { realizar_desvio(); pthread_exit(NULL); }

int main() {
    pthread_t t1, t2, t3;
    double inicio, fim;

    preencher_lista();
    printf("--- Lista gerada com %d inteiros [0-100] ---\n\n", TAM_LISTA);

    // ================= CASO 1: MULTI-THREAD =================
    printf(">> Executando com 3 THREADS...\n");
    inicio = pegar_tempo();

    // Criação
    pthread_create(&t1, NULL, thread_media, NULL);
    pthread_create(&t2, NULL, thread_mediana, NULL);
    pthread_create(&t3, NULL, thread_desvio, NULL);

    // Join (Espera terminar)
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    fim = pegar_tempo();
    
    printf("Média: %.2f | Mediana: %.2f | Desvio Padrão: %.2f\n", media_global, mediana_global, desvio_global);
    printf("Tempo TOTAL (Threads): %.6f segundos\n\n", fim - inicio);

    // Zerar globais para teste sequencial
    media_global = mediana_global = desvio_global = 0;

    // ================= CASO 2: SINGLE-THREAD (SEQUENCIAL) =================
    printf(">> Executando SEQUENCIALMENTE (1 Thread)...\n");
    inicio = pegar_tempo();

    realizar_media();
    realizar_mediana();
    realizar_desvio();

    fim = pegar_tempo();

    printf("Média: %.2f | Mediana: %.2f | Desvio Padrão: %.2f\n", media_global, mediana_global, desvio_global);
    printf("Tempo TOTAL (Sequencial): %.6f segundos\n", fim - inicio);

    return 0;
}