#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include "timer.h"

#define MAX_RANDOM_SIZE 9

typedef long long int lli;
typedef struct {
  int id;
  int start;
  int end;
} tArgs;

/* 
  Variaveis necess�ria para a execu��o do c�digo.
*/
int matSize, numThreads;
lli *matrix, *matrixAux, *matrixResult;
pthread_t *osId, *localId;
tArgs *params;

/* 
  Fun��o respons�vel por ler o input do usu�rio de uma forma amig�vel.
*/
void readInput() {
  printf("\n");
  printf("Digite o numero de threads: ");
  scanf("%d", &numThreads);
  printf("Digite o tamanho da matriz: ");
  scanf("%d", &matSize);
  printf("\n");
}

/* 
  Fun��o para reservar toda a mem�ria necess�ria, baseada no input do usu�rio.
*/
void reserveMemory () {
  matrix = malloc(sizeof(lli) * matSize * matSize);
  matrixAux = malloc(sizeof(lli) * matSize * matSize);
  matrixResult = malloc(sizeof(lli) * matSize * matSize);
  osId = malloc(sizeof(pthread_t) * numThreads);
  localId = malloc(sizeof(pthread_t) * numThreads);
  params = malloc(sizeof(tArgs) * numThreads);
  if(matrix == NULL || matrixAux == NULL || matrixResult == NULL) {
    printf("Falha na alocacao de memoria");
    exit(1);
  }
}

/* 
  Libera a mem�ria alocada.
*/
void freeMemory() {
  free(matrix);
  free(matrixAux);
  free(matrixResult);
  free(osId);
  free(localId);
  free(params);
}

/* 
  Retorna um n�mero inteiro aleat�rio, foi usado um n�mero pequeno para evitar overflows.
*/
int getRandomInt() {
  return (rand() % MAX_RANDOM_SIZE) + 1;
}

/* 
  Gera as duas matrizes aleat�rias.
*/
void genMatrix() {
	int i;
  for(i = 0; i < matSize; i++) {
  	int j;
    for(j = 0; j < matSize; j++) {
      matrix[i*matSize+j] = getRandomInt();
      matrixAux[i*matSize+j] = getRandomInt();
    }
  }
}

/* 
  Imprime uma matriz espec�fica.
*/
void printMatrix(lli *matrixTmp) {
	int i;
  for(i = 0; i < matSize; i++) {
  	int j;
    for(j = 0; j < matSize; j++) {
      printf("%lld ", matrixTmp[i*matSize+j]);
    }
    printf("\n");
  }
  printf("\n");
}

/* 
  Imprime as duas matrizes e a matriz resultante.
*/
void printMatrixs() {
  printMatrix(matrix);
  printMatrix(matrixAux);
  printMatrix(matrixResult);
}

/* 
  Fun��o que ser� chamada pelos Threads.
*/
void *cellMultiplication(void* arg) {
  tArgs *args = (tArgs*) arg;
  
  int i;

  for(i = args->start; i < args->end; i++) {
    lli count = 0;
    int line = i/matSize;
    int col = i - (line*matSize);
    int k;
    for(k = 0; k < matSize; k++) {
      count += matrix[line*matSize+k]*matrixAux[k*matSize+col];
    }
    matrixResult[line*matSize+col] = count;
  }
}

/* 
  Fun��o respons�vel por criar os Threads e passar seus respectivos par�metros.
*/
void matrixMultiplication() {
  int totalCells = (matSize * matSize);
  int qtdThread = totalCells / numThreads;
  int i;
  for(i = 0; i < numThreads; i++) {
    localId[i] = i;
    (params+i)->id = i;
    (params+i)->start = qtdThread * i;
    (params+i)->end = (i == numThreads - 1 ? totalCells: (params+i)->start + qtdThread); 

    if (pthread_create(&osId[i], NULL, cellMultiplication, (void*) (params+i))) {
      printf("Erro ao criar thread com id %d\n", i); exit(-1);
    }
  }
  

  for (i = 0; i < numThreads; i++) {
    if (pthread_join(osId[i], NULL)) {
      printf("Erro ao executar pthread_join\n"); exit(-1); 
    }
  }
}

/* 
  Fun��o que verifica se o resultado da anteior est� correto, usando o algoritmo n^3.
*/
void verifyResult() {
	int i;
  for(i = 0; i < matSize; i++) {
  	int j;
    for(j = 0; j < matSize; j++) {
      lli count = 0;
      int k;
      for(k = 0; k < matSize; k++) {
        count += matrix[i*matSize+k]*matrixAux[k*matSize+j];
      }
      assert(matrixResult[i*matSize+j] == count);
    }
  }
}

/* 
  A main
*/
int main(int argc, char const *argv[]) {
  srand(time(NULL));
  int running = 1;
  double begin, end, result;
  while(running) {
    readInput();

    GET_TIME(begin);
    reserveMemory();
    GET_TIME(end);
    result = end-begin;
    printf("Tempo reservando a memoria: %lf\n", result);

    GET_TIME(begin);
    genMatrix();
    GET_TIME(end);
    result = end-begin;
    printf("Tempo gerando matrizes: %lf\n", result);

    GET_TIME(begin);
    matrixMultiplication();
    GET_TIME(end);
    result = end-begin;
    printf("Tempo multiplicando com os threads: %lf\n", result);

    GET_TIME(begin);
    verifyResult();
    GET_TIME(end);
    result = end-begin;
    printf("Tempo verificando o resultado: %lf\n", result);

    GET_TIME(begin);
    freeMemory();
    GET_TIME(end);
    result = end-begin;
    printf("Tempo liberando a memoria: %lf\n", result);
  }

}
