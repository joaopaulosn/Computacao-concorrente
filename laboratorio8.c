#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

sem_t semaforo1, semaforo2;
int count = 0;


void *thread1 (void *params) {
  sem_wait(&semaforo1);
  printf("Fique a vontade.\n");
  if(count == 1) {
    sem_post(&semaforo2);
  } else {
    count++;
    sem_post(&semaforo1);
  }
  pthread_exit(NULL);
}


void *thread2 (void *params) {
  printf("Seja bem-vindo!\n");
  sem_post(&semaforo1);
  pthread_exit(NULL);
}


void *thread3 (void *params) {
  sem_wait(&semaforo2);
  printf("Volte sempre!\n");
  pthread_exit(NULL);
}


void *thread4 (void *params) {
  sem_wait(&semaforo1);
  printf("Sente-se por favor.\n");
  if(count == 1) {
    sem_post(&semaforo2);
  } else {
    count++;
    sem_post(&semaforo1);
  }
  pthread_exit(NULL);
}


int main(void) {
  pthread_t threads[4];
  int *threadsIds;
  threadsIds = malloc(sizeof(int) * 4);

  sem_init(&semaforo1, 0, 0);
  sem_init(&semaforo2, 0, 0);

	int i;
  for(i = 0; i < 4; i++) {
    threadsIds[i] = i;
  }

  pthread_create(&threads[0], NULL, thread2, (void *) &threadsIds[0]);
  pthread_create(&threads[1], NULL, thread1, (void *) &threadsIds[1]);
  pthread_create(&threads[2], NULL, thread4, (void *) &threadsIds[2]);
  pthread_create(&threads[3], NULL, thread3, (void *) &threadsIds[3]);
  
  
  for (i = 0; i < 4; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
