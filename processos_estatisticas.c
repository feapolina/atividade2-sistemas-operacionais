#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#define TAM_LISTA 10000

//funcao de comparacao dos elementos do array utilizada pelo quicksort
int comparar(const void* arg, const void* argb){
    int x = *(int*)arg;
    int y = *(int*)argb;
    
    if(x<y) return -1;
    if(x>y) return 1;
    return 0;
}

//funcao implementada para calcular a media e ser utilizada pelo processo1
float calculo_media(void* arg){
    float resultado_processo1 = 0;
    int *dados = (int*) arg;

    for(int i=0; i<TAM_LISTA; i++){
        resultado_processo1 += dados[i]; 
    }
    resultado_processo1 = resultado_processo1/TAM_LISTA;
    
    return resultado_processo1;
}

//funcao de calculo da mediana utilizada pela thread 2
float calculo_mediana(void* arg){
    float resultado_processo2 = 0;
    
    int *dados = (int*) arg;
    qsort(dados, TAM_LISTA, sizeof(int), comparar); //uso do algoritmo quicksort para ordenacao da lista para obter a mediana

    if ((TAM_LISTA%2) == 0){
        resultado_processo2 = (dados[(TAM_LISTA/2)-1] + dados[(TAM_LISTA/2)])/2; //resultado se o tamanho do vetor/lista for par
    }
    else{
        resultado_processo2 = (dados[(TAM_LISTA/2)]); //resultado se for impar
    }
    
    return resultado_processo2;    
}

float calculo_desvio_padrao(void* arg){
    float resultado_processo3 = 0;
    
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
    
    resultado_processo3 = sqrt(soma/TAM_LISTA); //resultado armazenado em uma variavel global  

    return resultado_processo3;
}

int main(){
    float tempo_exec_processo1 = 0;
    float tempo_exec_processo2 = 0;
    float tempo_exec_processo3 = 0;
    float tempo_criacao_processo1 = 0;
    float tempo_criacao_processo2 = 0;
    float tempo_criacao_processo3 = 0;

    struct timespec start, end;
    srand(time(NULL));
    int pipe_media[2];
    int pipe_mediana[2];
    int pipe_desvio_padrao[2];
    int pipe_t_exec_1[2];
    int pipe_t_exec_2[2];
    int pipe_t_exec_3[2];

    pipe(pipe_media);
    pipe(pipe_mediana);
    pipe(pipe_desvio_padrao);
    pipe(pipe_t_exec_1);
    pipe(pipe_t_exec_2);
    pipe(pipe_t_exec_3);
       
    int dados[TAM_LISTA];

    //cria a lista com 10.000 inteiros
    for(int i=0;i<TAM_LISTA;i++){
        dados[i] = rand() % 101;
    }

    //cria os processos filhos
    clock_gettime(CLOCK_MONOTONIC, &start);
    pid_t pid1 = fork();
    clock_gettime(CLOCK_MONOTONIC, &end);
    tempo_criacao_processo1 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec);

    if(pid1 == 0){
        clock_gettime(CLOCK_MONOTONIC, &start); //marca o tempo de inicio da thread usando CLOCK_MONOTONIC para mais fidelidade
        float resultado1 = 0;
        close(pipe_media[0]);
        close(pipe_t_exec_1[0]);
        resultado1 = calculo_media((void*)dados);
        write(pipe_media[1], &resultado1, sizeof(float));
        close(pipe_media[1]);
        clock_gettime(CLOCK_MONOTONIC, &end); //marca o fim da execucao da thread
        tempo_exec_processo1 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec); //calculo do tempo de execucao convertido para nanosegundos
        write(pipe_t_exec_1[1], &tempo_exec_processo1, sizeof(float));
        close(pipe_t_exec_1[1]);
        exit(0);
    }
    

    clock_gettime(CLOCK_MONOTONIC, &start);
    pid_t pid2 = fork();
    clock_gettime(CLOCK_MONOTONIC, &end);
    tempo_criacao_processo2 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec);

    if(pid2 == 0){
        clock_gettime(CLOCK_MONOTONIC, &start);    
        float resultado2 = 0;
        close(pipe_mediana[0]);
        close(pipe_t_exec_2[0]);
        resultado2 = calculo_mediana((void*) dados);
        write(pipe_mediana[1], &resultado2, sizeof(float));
        close(pipe_mediana[1]);
        clock_gettime(CLOCK_MONOTONIC, &end);
        tempo_exec_processo2 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec); //calculo do tempo de execucao da thread 2 convetido para nanosegundos
        write(pipe_t_exec_2[1], &tempo_exec_processo2, sizeof(float));
        close(pipe_t_exec_2[1]);
        exit(0);
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    pid_t pid3 = fork();
    clock_gettime(CLOCK_MONOTONIC, &end);
    tempo_criacao_processo3 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec);

    if(pid3 == 0){
        clock_gettime(CLOCK_MONOTONIC, &start);
        float resultado3 = 0;
        close(pipe_desvio_padrao[0]);
        close(pipe_t_exec_3[0]);
        resultado3 = calculo_desvio_padrao((void*)dados);
        write(pipe_desvio_padrao[1], &resultado3, sizeof(float));
        close(pipe_desvio_padrao[1]);
        clock_gettime(CLOCK_MONOTONIC, &end);
        tempo_exec_processo3 = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec);//tempo de execucao da thread 3 convertido em nanosegundos
        write(pipe_t_exec_3[1], &tempo_exec_processo3, sizeof(float));
        close(pipe_t_exec_3[1]);
        exit(0);
    }

    if(pid1 > 0){
        wait(NULL);
    }

    if(pid2 > 0){
        wait(NULL);
    }

    if(pid3 > 0){
        wait(NULL);
    }

    close(pipe_media[1]);
    close(pipe_mediana[1]);
    close(pipe_desvio_padrao[1]);
    close(pipe_t_exec_1[1]);
    close(pipe_t_exec_2[1]);
    close(pipe_t_exec_3[1]);

    float resultado_processo1, resultado_processo2, resultado_processo3 = 0;

    read(pipe_media[0], &resultado_processo1, sizeof(float));
    read(pipe_mediana[0], &resultado_processo2, sizeof(float));
    read(pipe_desvio_padrao[0], &resultado_processo3, sizeof(float));
    read(pipe_t_exec_1[0], &tempo_exec_processo1, sizeof(float));
    read(pipe_t_exec_2[0], &tempo_exec_processo2, sizeof(float));
    read(pipe_t_exec_3[0], &tempo_exec_processo3, sizeof(float));

    printf("RESULTADO MEDIA = %.2f | RESULTADO MEDIANA = %.2f | RESULTADO DESVIO PADRAO = %.2f\n\n", resultado_processo1, resultado_processo2, resultado_processo3);
    printf("TEMPO CRIACAO PROCESSO 1 = %.2f | TEMPO CRIACA PROCESSO 2 = %.2f | TEMPO CRIACAO PROCESSO 3 = %.2f\n\n", tempo_criacao_processo1, tempo_criacao_processo2, tempo_criacao_processo3);
    printf("TEMPO EXECUCAO PROCESSO 1 = %.2f | TEMPO EXECUCAO PROCESSO 2 = %.2f | TEMPO EXECUCAO PROCESSO 3 = %.2f\n\n", tempo_exec_processo1, tempo_exec_processo2, tempo_exec_processo3);

    return 0;
}