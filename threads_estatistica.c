#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <pthread.h> //biblioteca para gerenciamento das threads
#include <gsl/gsl_statistics.h> //biblioteca usada para iniciar a semente do rand()
#include <time.h> //biblioteca utilizada para calcular os tempos de criação e execução das threads
#include <math.h> //biblioteca utilizada para calculos matematicos de potencia e raiz quadrada
#include <unistd.h> //usado para padronizar o uso do CLOCK_MONOTONIC com o sistema POSIX

#define TAM_LISTA 10000

//variaveis globais que as threads tem acesso
float resultado_thread1 = 0; 
float resultado_thread2 = 0;
float resultado_thread3 = 0;
long long tempo_criacao_t1 = 0;
long long tempo_criacao_t2 = 0;
long long tempo_criacao_t3 = 0;
long long tempo_exec_t1 = 0;
long long tempo_exec_t2 = 0;
long long tempo_exec_t3 = 0;

//funcao de comparacao dos elementos do array utilizada pelo quicksort
int comparar(const void* arg, const void* argb){
    int x = *(int*)arg;
    int y = *(int*)argb;
    
    if(x<y) return -1;
    if(x>y) return 1;
    return 0;
}

//funcao implementada para calcular a media e ser utilizada pela thread1
void* calculo_media(void* arg){
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start); //marca o tempo de inicio da thread usando CLOCK_MONOTONIC para mais fidelidade
    int *dados = (int*) arg;
    for(int i=0; i<TAM_LISTA; i++){
        resultado_thread1 += dados[i]; 
    }
    
    resultado_thread1 = resultado_thread1/TAM_LISTA;
    clock_gettime(CLOCK_MONOTONIC, &end); //marca o fim da execucao da thread

    tempo_exec_t1 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec); //calculo do tempo de execucao convertido para nanosegundos
}

//funcao de calculo da mediana utilizada pela thread 2
void* calculo_mediana(void* arg){
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int *dados = (int*) arg;
    qsort(dados, TAM_LISTA, sizeof(int), comparar); //uso do algoritmo quicksort para ordenacao da lista para obter a mediana

    if ((TAM_LISTA%2) == 0){
        resultado_thread2 = (dados[(TAM_LISTA/2)-1] + dados[(TAM_LISTA/2)])/2; //resultado se o tamanho do vetor/lista for par
    }
    else{
        resultado_thread2 = (dados[(TAM_LISTA/2)]); //resultado se for impar
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    tempo_exec_t2 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec); //calculo do tempo de execucao da thread 2 convetido para nanosegundos
}

void* calculo_desvio_padrao(void* arg){
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    int *dados = (int*) arg;
    int soma = 0;
    float media = 0;
    float quadrado[TAM_LISTA];
    float desvio_padrao = 0;

    for(int i=0;i<TAM_LISTA;i++){
        soma += dados[i];//somando todos os elementos para obter o desvio padrao populacional
    }

    media = (float)soma / TAM_LISTA; 
    
    for(int i=0;i<TAM_LISTA;i++){
        quadrado[i] = (pow((dados[i]-media),2)); //subtracao dos elementos pela media elevado ao quadrado
    }

    soma = quadrado[0]; //defino o valor de soma como o primeiro elemento dos quadrados para ser somado dentro do for para obter a soma dos quadrados

    for(int i = 1 ; i<TAM_LISTA; i++){
        soma += quadrado[i]; //soma dos quadrados
    }
    
    resultado_thread3 = sqrt(soma/TAM_LISTA); //resultado armazenado em uma variavel global
    clock_gettime(CLOCK_MONOTONIC, &end);

    tempo_exec_t3 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec);//tempo de execucao da thread 3 convertido em nanosegundos
}

int main(){
    struct timespec start, end; //inicializacao da estrutura necessaria para o calculo das threads para calcular o tempo de criacao das threads
    int dados[TAM_LISTA];

    srand(time(NULL));//semente de geração dos números aleatórios

    for(int i=0;i<TAM_LISTA;i++){
        dados[i] = rand() % 101; //geracao dos numeros aleatorios
    }

    
    pthread_t t1;
    pthread_t t2;
    pthread_t t3;

    clock_gettime(CLOCK_MONOTONIC, &start); //marca o inicio da criacao da thread 1
    pthread_create(&t1, NULL, calculo_media, (void*)dados); //cria a thread 1 passando a lista como ultimo argumento da funcao de criacao da thread 1
    clock_gettime(CLOCK_MONOTONIC, &end);

    tempo_criacao_t1 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec); //calculo do tempo de criacao da thread 1

    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&t2, NULL, calculo_mediana, (void*)dados);
    clock_gettime(CLOCK_MONOTONIC, &end);

    tempo_criacao_t2 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&t3, NULL, calculo_desvio_padrao, (void*)dados);
    clock_gettime(CLOCK_MONOTONIC, &end);

    tempo_criacao_t3 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec);

    //definido para aguardar as threads finalizarem a execucao para dar continuidade com a thread principal
    pthread_join(t1, NULL); 

    pthread_join(t2, NULL);

    pthread_join(t3, NULL);
    
    printf("EXECUCAO COM 3 THREADS:\n\n");

    printf("Resultado da media: %0.2f\n", resultado_thread1);
    printf("Resultado da mediana: %0.2f\n", resultado_thread2);
    printf("Resultado do desvio padrao: %0.2f\n\n\n", resultado_thread3);
    
    printf("tempo de criação thread 1: %lld\n", tempo_criacao_t1);
    printf("tempo de criação thread 2: %lld\n", tempo_criacao_t2);
    printf("tempo de criação thread 3: %lld\n\n\n", tempo_criacao_t3);

    printf("tempo de execucao thread 1: %lld\n", tempo_exec_t1);
    printf("tempo de execucao thread 2: %lld\n", tempo_exec_t2);
    printf("tempo de execucao thread 3: %lld\n\n\n", tempo_exec_t3);

    printf("EXECUCAO SEQUENCIAL (1 THREAD = 1 PROCESSO): \n\n");


    //clock_gettime foi usado novamente para calcular inicio, fim e depois foi feito o calculo do tempo e armazenado na variavel local
    clock_gettime(CLOCK_MONOTONIC, &start);
    calculo_media((void*)dados);
    clock_gettime(CLOCK_MONOTONIC, &end);
    float tempo_exec_media = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC, &start);
    calculo_mediana((void*)dados);
    clock_gettime(CLOCK_MONOTONIC, &end);
    float tempo_exec_mediana = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC, &start);
    calculo_desvio_padrao((void*)dados);
    clock_gettime(CLOCK_MONOTONIC, &end);
    float tempo_exec_desvio_padrao = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

    printf("Resultado da media: %0.2f\n", resultado_thread1);
    printf("Resultado da mediana: %0.2f\n", resultado_thread2);
    printf("Resultado do desvio padrao: %0.2f\n\n", resultado_thread3);

    printf("tempo execucao media = %0.2f\n", tempo_exec_media);
    printf("tempo execucao mediana = %0.2f\n", tempo_exec_mediana);
    printf("tempo execucao desvio padrao = %0.2f\n\n", tempo_exec_desvio_padrao);

    return 0;
}