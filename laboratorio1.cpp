#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <time.h>



long long int array[10000];


void *potencia(void* arg) {
  int idThread = * (int*) arg;
  printf("Executando thread %d...\n", idThread);

if (idThread % 2){
	
  for (int i = 0; i< 10000; i=i+2){  //pega os indicies pares
  		array[i] *=array[i];
  		}
  	}
  	else{
  		for (int i = 1; i< 10000; i= i+2){  //pega os índices impares
  		array[i] *= array[i];	
		  }
	  }


  pthread_exit(NULL);
}


  //Gerando vetor com 10000 valores aleatorios.

void genArray() {
  for(int i = 0; i < 10000; i++) {
    array[i] = abs(rand() % 50000) + 1;
  }
}


void printArray() {
  for(int i = 0; i < 10000; i++) {
    printf("%lld ", array[i]);
    if(i > 0 && i % 10000 == 0) printf("\n");
  }
  printf("\n");
}

int main(void){
  srand(time(NULL)); 

  pthread_t idOs[2]; // Id das threads
  int idLocal[2]; // Id local dos threads.
  long long int generatedArray[10000]; // Uma cópia do vetor original, para que a gente possa comparar se o resultado está realmente correto.
  
  printf("imprimindo valores aleatorios: \n\n\n");
  genArray();
  printArray();
  
  memcpy(generatedArray, array, sizeof(generatedArray));

  // Inicia os threads chamando a função potencia.
  for(int i = 0; i < 2; i++) {
    idLocal[i] = i + 1;
    if (pthread_create(&idOs[i], NULL, potencia, (void*) &idLocal[i])) {
      printf("Erro ao criar thread com id %d\n", idLocal[i]); exit(-1);
    }
  }

  // Aguarda o retorno das threads.
  for (int i = 0; i < 2; i++) {
    if (pthread_join(idOs[i], NULL)) {
      printf("Erro ao executar pthread_join\n"); exit(-1); 
    }
  }

  // Verifica se o programa está executando com os valores corretos, caso algum valor esteja errado, o print não será executado. 
  for(int i = 0; i < 10000; i++) {
    assert(generatedArray[i] * generatedArray[i] == array[i]);
  }

  printArray();

  return 0;
}
