#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include "timer.h"

/* 
  * Variáveis globais e defines
*/
#define VECTOR_SIZE 12 // Tamanho máximo do vetor.
#define NUM_THREAD 4 // Número de threads.
#define MAX_VALUE 100 // Valor máximo dos elementos do vetor.

// Tipo de dados que será usado como parâmetro para cada thread.
typedef struct {
  int low;
  int high;
} tArgs;
int *vector, *cloneVector, *localId; // Vetor original e um clone.
tArgs *params; // Vetor que guarda os parâmetros do theads.
pthread_t *threads; // Threads que serão usados pelo algoritmo.

/* 
  * Funções auxiliares
  Essas funções serão usadas pelo algoritmo sequencial e paralelo.
*/

/* 
  Reserva a memória para os vetores.
*/
void reserveMemory() {
  vector = malloc(sizeof(int) * VECTOR_SIZE);
  cloneVector = malloc(sizeof(int) * VECTOR_SIZE);
  localId = malloc(sizeof(int) * NUM_THREAD);
  threads = malloc(sizeof(pthread_t) * NUM_THREAD);
  params = malloc(sizeof(tArgs) * NUM_THREAD);

  if(vector == NULL || cloneVector == NULL || threads == NULL || params == NULL) {
    printf("Falha ao alocar memoria!");
    exit(1);
  }
}

/* 
  Retorna um número aleatório entre 1 e o MAX_VALUE.
*/
int genRandomInt() {
  return rand() % (MAX_VALUE+1);
}

/* 
  Gera um vetor aleatório.
*/
void genRandomVector() {
  for(int i = 0; i < VECTOR_SIZE; i++) {
    int sorted = genRandomInt();
    vector[i] = cloneVector[i] = sorted;
  }
}

/* 
  Imprime o vector.
*/
void printVector(int *arr) {
  for(int i = 0; i < VECTOR_SIZE; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

/* 
  Testa se ambos os vetores estão ordenados.
*/
void test() {
  for(int i = 0; i < VECTOR_SIZE - 1; i++) {
    if(vector[i] > vector[i+1]) {
      printf("O algoritmo paralelo não ordenou corretamente!\n");
      exit(1);
    }
    if(cloneVector[i] > cloneVector[i+1]) {
      printf("O algoritmo sequencial não ordenou corretamente!\n");
      exit(1);
    }
  }
}

/* 
  * Merge Paralelo
*/

/* 
  Função merge, essa função realiza o merge entre 2 vetores
  de modo que o vetor gerado por eles estejam ordenados.
*/
void merge(int low, int mid, int high) {
  // N1 é o tamanho do vetor left e N2 é o tamanho do vetor da direita.
  // I e J são variáveis que serão utilizadas para iterar os vetores.
  int n1 = mid - low + 1, n2 = high - mid, i, j;

  // Alocando espaço para o vetor da esquerda e da direita.
  int *left = malloc(sizeof(int) * (n1));
  int *right = malloc(sizeof(int) * (n2));

  if(left == NULL || right == NULL) {
    printf("Falha ao alocar memoria!\n");
    exit(1);
  }

  // Armazena os valores no vetor da esquerda.
  for (i = 0; i < n1; i++) {
    left[i] = vector[i + low];
  }

  // Armazena os valores no vetor da direita.
  for (i = 0; i < n2; i++) {
    right[i] = vector[i + mid + 1];
  }

  int k = low;
  i = j = 0;

  // Realiza o merge do vetor left e right.
  while (i < n1 && j < n2) {
    if (left[i] <= right[j]) {
      vector[k++] = left[i++];
    } else {
      vector[k++] = right[j++];
    }
  }

  // Insere os valores restantes do left.
  while (i < n1) {
    vector[k++] = left[i++];
  }

  // Insere os valores restantes do right.
  while (j < n2) {
    vector[k++] = right[j++];
  }
}

/*  
  Algoritmo de Mergesort.
*/
void merge_sort(int low, int high) {
  // Divide o vetor ao meio.
  int mid = low + (high - low) / 2;
  if (low < high) {
    // Realizado o merge no vetor da esquerda.
    merge_sort(low, mid);
    // Realiza o merge no vetor da direita.
    merge_sort(mid + 1, high);
    // Realiza o merge entre o lado esquerdo e direito.
    merge(low, mid, high);
  }
}

/* 
  Worker que será executado por cada thread.
  O Worker recebe apenas um inteiro como parâmetro que é a posição do vetor params.
*/
void *merge_sort_worker(void *arg) {
  int position = *(int*) arg;

  int low = (params+position)->low;
  int high = (params+position)->high;
  int mid = low + (high-low)/2;

  if (low < high) {
    merge_sort(low, mid);
    merge_sort(mid + 1, high);
    merge(low, mid, high);
  }
}

/* 
  Executa a função mergeSort em paralelo.
  Essa função é responsável por criar os threads, aguardar sua execução e juntar os valores gerados por cada thread.
*/
void executeMergeSortParallel() {
  // Cria os threads.
  for (int i = 0; i < NUM_THREAD; i++) {
    int low = i * (VECTOR_SIZE / NUM_THREAD);
    int high = (i + 1) * (VECTOR_SIZE/NUM_THREAD) - 1;

    localId[i] = i;
    (params+i)->low = low;
    (params+i)->high = high;

    pthread_create(&threads[i], NULL, merge_sort_worker, (void*) &localId[i]);
  }

  // Aguarda o fim dos threads.
  for (int i = 0; i < 4; i++) {
    pthread_join(threads[i], NULL);
  }

  int n = VECTOR_SIZE;
  // merge(0, (n / 2 - 1) / 2, n / 2 - 1);
  // merge(n / 2, n/2 + (n-1-n/2)/2, n - 1);
  // merge(0, (n - 1)/2, n - 1);

  // ! Tentativa fracassada de deixar o algoritmo escalável.
  // Realiza o merge do resultado de cada thread.
  for(int i = NUM_THREAD; i > 1; i /= 2) {
    for(int j = 0; j < i; j += 2) {
      int low = j * (VECTOR_SIZE/i);
      int high = (j + 2) * (VECTOR_SIZE/i) - 1;

      if(j + 2 >= i) {
        high = VECTOR_SIZE - 1;
      }
      int mid = low + (high - low)/2;
      merge(low, mid, high);
    }
  }
}


/* 
  * Merge sort sequencial
*/

/* 
  Mergesort sequencial, como ele segue os mesmo principios do mergesorte acima, ele não será comentados 
  com tanto detalhes, achamos que deixar o código mais clean duplicando as funções.
*/
void mergeSequencial(int l, int m, int r) {
  int i, j, k;
  int n1 = m - l + 1;
  int n2 = r - m;

  // Cria vetores temporários.
  int *L = malloc(sizeof(int) * (n1));
  int *R = malloc(sizeof(int) * (n2));

  // Divide os dois valores.
  for(int i = 0; i < n1; i++) {
    L[i] = cloneVector[l + i];
  }
  for (j = 0; j < n2; j++) {
    R[j] = cloneVector[m + 1 + j];
  }

  
  i = j = 0;
  k = l;

  // Realiza o merge entre L e R.
  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      cloneVector[k] = L[i++];
    } else {
      cloneVector[k] = R[j++];
    }
    k++;
  }

  while (i < n1) {
    cloneVector[k++] = L[i++];
  }

  while (j < n2) {
    cloneVector[k++] = R[j++];
  }
}

void mergeSort(int l, int r) {
  if (l < r) {
    int m = l + (r - l) / 2;
    mergeSort(l, m);
    mergeSort(m + 1, r);
    mergeSequencial(l, m, r);
  }
}

// A Main
int main() {
  srand(time(NULL));
  double start, finish, paralelo, sequencial;
  
  GET_TIME(start);
  reserveMemory();
  GET_TIME(finish);
  // printf("Tempo para reservar memoria: %.9lf\n", finish-start);

  GET_TIME(start);
  genRandomVector();
  GET_TIME(finish);
  // printf("Tempo para gerar o vetor: %.9lf\n", finish-start);

  GET_TIME(start);
  executeMergeSortParallel();
  GET_TIME(finish);
  paralelo = finish - start;
  printf("Tempo total paralelo: %.9lf\n", paralelo);


  GET_TIME(start);
  mergeSort(0, VECTOR_SIZE - 1);
  GET_TIME(finish);
  sequencial = finish - start;
  printf("Tempo total sequencial: %.9lf\n", sequencial);

  GET_TIME(start);
  test();
  GET_TIME(finish);
  printf("Tempo total testando: %.9lf\n", finish-start);

  printf("Ganho de performance: %.2lf %% \n", (paralelo/sequencial)-1);



  return 0;
}