#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include "timer.h"

#define MAX_RANDOM_SIZE 1000000
#define INF (1 << 30)

typedef long long int lli;

typedef struct {
  int id;
  int start;
  int end;
} tArgs;

typedef struct {
  float min;
  float max;
} tResult;

/* 
  Variaveis necessária para a execução do código.
*/
int vectorSize, numThreads;
float *vector;
pthread_t *osId, *localId;
tArgs *params;

// Resposta do problema que será calculado de forma paralela.
float menor, maior;

/* 
  Função responsável por ler o input do usuário de uma forma amigável.
*/
void readInput() {
  printf("\n");
  printf("NUM DE THREADS: ");
  scanf("%d", &numThreads);
  printf("TAMANHO DO VETOR: ");
  scanf("%d", &vectorSize);
  printf("\n");
}

// alocação de memoria

void reserveMemory () {
  menor = INF, maior = -1;
  vector = malloc(sizeof(float) * vectorSize);
  osId = malloc(sizeof(pthread_t) * numThreads);
  localId = malloc(sizeof(pthread_t) * numThreads);
  params = malloc(sizeof(tArgs) * numThreads);
  if(vector == NULL || osId == NULL || localId == NULL || params == NULL) {
    printf("Erro ao alocar memoria");
    exit(1);
  }
}

// Libera a memoria

void freeMemory() {
  free(vector);
  free(osId);
  free(localId);
  free(params);
}

// Gera um float aleatorio

float getRandomFloat() {
  return ((rand() % MAX_RANDOM_SIZE) + 1) / 100.0;
}


  //Gera uma lista com números aleatórios.

void genVector() {
	int i;
  for(i = 0; i < vectorSize; i++) {
    vector[i] = getRandomFloat();
  }
}

// Imprime o vetor gerado.

void printVector() {
	int i;
  for(i = 0; i < vectorSize; i++) {
    printf("%lf ", vector[i]);
  }
  printf("\n");
}

// Função para comparar ponto flutuante de uma forma mais precisa.

int floatCompare(float a, float b){
    float eps=1e-5;
    if(b+eps<a) return 1;
    if(a+eps<b) return -1;
    return 0;
}

// Função que será chamada pelo threads, ela calcula o menor e maior valor
// dada uma certa range e retorna usando o exit.
void *getMinAndMax(void* arg) {
  tArgs *args = (tArgs*) arg;
  tResult *result = malloc(sizeof(tResult));

  result->min = INF;
  result->max = -1;
  int i;
  for(i = args->start; i < args->end; i++) {
    if(floatCompare(vector[i], result->min) == -1) result->min = vector[i];
    if(floatCompare(vector[i], result->max) == 1) result->max = vector[i];
  }

  pthread_exit((void*) result);
}

// Função que controla os threads e divide o vetor entre eles, além de calcular a resposta 
// correta com base no retorno de cada thread.
void solveUsingThreads() {
  int qtdPerThread = vectorSize / numThreads;
  int i;
  for(i = 0; i < numThreads; i++) {
    localId[i] = i;
    (params+i)->id = i;
    (params+i)->start = qtdPerThread * i;
    (params+i)->end = (i == numThreads - 1 ? vectorSize : (params+i)->start + qtdPerThread); 

    if (pthread_create(&osId[i], NULL, getMinAndMax, (void*) (params+i))) {
      printf("Erro ao criar thread com id %d\n", i); exit(-1);
    }
  }
// a variavel i desse for ja foi declarada antes
  for (i = 0; i < numThreads; i++) {
    void *returningValue;
    tResult *result;

    if (pthread_join(osId[i], &returningValue)) {
      printf("Erro ao executar pthread_join\n"); exit(-1); 
    }

    result = (tResult*) returningValue;
    if(floatCompare(result->min, menor) == -1) menor = result->min;
    if(floatCompare(result->max, maior) == 1) maior = result->max;
    free(result);
  }
}

// Resolve o problema de forma sequencial e confere o resultado com o valor encontrado
// pelos threads.
void solveUsingOneThread() {
  float tmpMax = -1, tmpMin = INF;
  int i;
  for(i = 0; i < vectorSize; i++) {
    if(floatCompare(vector[i], tmpMin) == -1) tmpMin = vector[i];
    if(floatCompare(vector[i], tmpMax) == 1) tmpMax = vector[i];
  }

  assert(floatCompare(tmpMax, maior) == 0);
  assert(floatCompare(tmpMin, menor) == 0);
}


int main(int argc, char const *argv[]) {
  srand(time(NULL));
  int running = 1;
  double begin, end;
  while(running) {
    readInput();
    reserveMemory();
    genVector();

    GET_TIME(begin);
    solveUsingThreads();
    GET_TIME(end);
    double time1 = end-begin;
    printf("TEMPO EM PARALELO: %lf\n", time1);

    GET_TIME(begin);
    solveUsingOneThread();
    GET_TIME(end);
    double time2 = end-begin;
    printf("TEMPO SEQUENCIAL: %lf\n", time2);

    printf("ACELERACAO: %lf\n", time2/time1);

    freeMemory();
  }
}
