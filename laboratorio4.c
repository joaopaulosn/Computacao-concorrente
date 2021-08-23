#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_cond_t cond, condEnd;

int control = 0; // controlador do fluxo de execução


void *thread1 (void *params) {

  pthread_mutex_lock(&mutex);
  
  if(control != 1 && control != 2) {
    pthread_cond_wait(&cond, &mutex);
  }

  printf("Fique a vontade.\n");
  control++;
  
  // Caso o controle seja 3, falta apenas a ultimo thread para ser executada.
  if(control == 3) pthread_cond_signal(&condEnd);
  else pthread_cond_signal(&cond);

  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}



void *thread2 (void *params) {
  pthread_mutex_lock(&mutex);
  if(control == 0) {
    printf("Seja bem-vindo!\n");
    control++;
    pthread_cond_signal(&cond);
  }
  pthread_mutex_unlock(&mutex);
  pthread_exit(NULL);
}



void *thread3 (void *params) {
  pthread_mutex_lock(&mutex);
  if(control != 3) {
    pthread_cond_wait(&condEnd, &mutex);
  }
  printf("Volte sempre!\n");
  control++;
  pthread_mutex_unlock(&mutex);
  pthread_exit(NULL);
}



void *thread4 (void *params) {

  pthread_mutex_lock(&mutex);
  
  if(control != 1 && control != 2) {
    pthread_cond_wait(&cond, &mutex);
  }

  printf("Sente-se por favor.\n");
  control++;
  if(control == 3) pthread_cond_signal(&condEnd);
  else pthread_cond_signal(&cond);

  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}


int main(void) {
  pthread_t threads[4];
  int *threadsIds;
  threadsIds = malloc(sizeof(int) * 4);

  // Criando as condições e o lock.
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init (&cond, NULL);
  pthread_cond_init (&condEnd, NULL);
	int i;
  for(i = 0; i < 4; i++) {
    threadsIds[i] = i;
  }

  // Criando as threads.
  pthread_create(&threads[0], NULL, thread1, (void *) &threadsIds[0]);
  pthread_create(&threads[1], NULL, thread2, (void *) &threadsIds[1]);
  pthread_create(&threads[2], NULL, thread4, (void *) &threadsIds[2]);
  pthread_create(&threads[3], NULL, thread3, (void *) &threadsIds[3]);
  
  for (i = 0; i < 4; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);

  return 0;
}
