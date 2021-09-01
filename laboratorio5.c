#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<time.h>

int *vector, size, *params, count = 0;
pthread_t *threads;  
pthread_mutex_t lock;
pthread_cond_t cond;

/* 
  Reserva a memória para os vetores e inicia o mutex e cond.
*/
void reserveMemory(int qtd) {
  size = qtd;
  vector = malloc(sizeof(int) * size);
  params = malloc(sizeof(int) * size);
  threads = malloc(sizeof(pthread_t) * size);
  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&cond, NULL);
}

/* 
  Limpa as memórias alocadas.
*/
void freeMemory() {
  free(vector);
  free(params);
  free(threads);
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&cond);
}

/* 
  Gera um número aleatório entre 0 e 9.
*/
int getRandomInt() {
  return rand() % 10;
}

/* 
  Gera um vetor de números aleatórios.
*/
void genRandomVector() {
	int i;
  for(i = 0; i < size; i++) {
    vector[i] = getRandomInt();
  }
}

/* 
  Imprime o vetor vector.
*/
void printVector() {
	int i;
  for(i = 0; i < size; i++) {
    printf("%d ", vector[i]);
  }
  printf("\n");
}

/* 
  Função que será executada por todos os threads.
*/
void *execute(void* arg) {
  int id = *(int*) arg;
  int *acc = 0;

  pthread_mutex_lock(&lock);
	int i;
  for(i = 0; i < size; i++) {
  	int i;
    for(i = 0; i < size; i++) {
      acc += vector[i];
    }
    count++;
    if(count != size) {
      pthread_cond_wait(&cond, &lock);
    } else {
      pthread_cond_broadcast(&cond);
      count = 0;
      vector[id] = getRandomInt();
    }
  }

  // printf("Saindo com soma acumulada de %d!\n", (int) acc);

  pthread_mutex_unlock(&lock);

  pthread_exit((void*) acc);
}

/* 
  A main
*/
int main(int argc, char const *argv[]) {
  srand(time(NULL));

  if(argc >= 2) {
    reserveMemory(atoi(argv[1]));
  } else {
    printf("Informe a quantidade de threads como argumento\n");
    exit(1);
  }

  genRandomVector();
	int i;
  for(i = 0; i < size; i++) {
    params[i] = i;

    if(pthread_create(&threads[i], NULL, execute, (void*) &params[i])) {
      printf("Falha ao criar thread com ID = %d", i);
      exit(1);
    }
  }


  int *last = NULL;
  for(i = 0; i < size; i++) {
    void *returningValue;

    if (pthread_join(threads[i], &returningValue)) {
      printf("Erro ao executar pthread_join\n");
      exit(-1); 
    }

    printf("Valor recebido pela thread %d = %d\n", i, returningValue);

    if(last == NULL) {
      last = (int*) returningValue;
    } else if(last != returningValue) {
      printf("As somas nao sao iguais!\n");
      exit(1);
    }
  }

  printf("As somas sao iguais a %d!\n", last);
  freeMemory();

  return 0;
}
