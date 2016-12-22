/*-----------------------------------------------------------------/
Universidade Federal de São Carlos
Curso: Laboratórios de Sistemas Operacionais

Projeto 4 - Programacao Concorrente
  Entrada: Uma hash
  Saida: A sennha correspondente
  O programa quebra senhas de 4 caracteres a partir de um hash

Desenvolvedoras: Ângela Ferreira - 552070
                 Isabela Salmeron - 552593

Outubro, 2015
/-----------------------------------------------------------------*/

/*-------------------------Bibliotecas----------------------------*/
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <crypt.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
/*--------------------------Definicoes----------------------------*/
#define MAX 10 
#define TAM_HASH 256
#define TAM_SENHA 4
/*----------------------Variaveis Globais-------------------------*/
//Fila em que o produtor guarda as senhas
char buffer[MAX][5]; //guarda MAX senhas de tam TAM_SENHA
//Variaveis do semaforo
sem_t bloqueio, pos_vazia, pos_ocupada;
//Auxiliam na organizacao da fila
int final=0, inicio=0;
//String que guarda cada uma das senhas geradas
char senha[TAM_SENHA+1];
//Guarda o hash de entrada
char *entrada;

/* Obtém o hash a partir de uma senha e coloca o resultado em hash.
   O vetor hash deve ter pelo menos 14 elementos. */
void calcula_hash_senha(const char *cod, char *hash);
void incrementa_senha();
void testa_senha(const char *cod);
void *produtor(void *v);
void *consumidor(void *v);
//void *consumidor2(void *v);

int main(int argc, char *argv[]) {
  int i;
  //char senha[TAM_SENHA + 1];
  entrada = argv[1];
  //Declaracao das threads
  pthread_t thr_prod, thr_cons1, thr_cons2, thr_cons3, thr_cons4, thr_cons5;
  //Inicializacao do semaforo
  sem_init(&bloqueio, 0, 1);
  sem_init(&pos_vazia, 0, MAX);
  sem_init(&pos_ocupada, 0, 0);


  if (argc != 2) {
    printf("Uso: %s <hash>", argv[0]);
    return 1;
  }

  for (i = 0; i < TAM_SENHA; i++) {
    senha[i] = 'a';
  }
  senha[TAM_SENHA] = '\0';

  //Criacao das threads
  pthread_create(&thr_prod, NULL, produtor, NULL);
  pthread_create(&thr_cons1, NULL, consumidor, NULL);
  pthread_create(&thr_cons2, NULL, consumidor, NULL);
  pthread_create(&thr_cons3, NULL, consumidor, NULL);
  pthread_create(&thr_cons4, NULL, consumidor, NULL);
  pthread_create(&thr_cons5, NULL, consumidor, NULL);


  pthread_join(thr_prod, NULL);
  pthread_join(thr_cons1, NULL); 
  pthread_join(thr_cons2, NULL); 
  pthread_join(thr_cons3, NULL);
  pthread_join(thr_cons4, NULL);
  pthread_join(thr_cons5, NULL);
  return 0;
}

void testa_senha(const char *cod) {
  char hash_calculado[TAM_HASH + 1];
  calcula_hash_senha(cod, hash_calculado);
  if (!strcmp(entrada, hash_calculado)) {
    printf("Achou! %s\n", cod);
    exit(0);
  }
}

void incrementa_senha() {
  int i;

  i = TAM_SENHA - 1;
  while (i >= 0) {
    if (senha[i] != 'z') {
      senha[i]++;
      i = -2;
    } else {
      senha[i] = 'a';
      i--;
    }
  }
  
  if (i == -1) {
    printf("Não achou!\n");
    exit(1);
  }
}

void calcula_hash_senha(const char *cod, char *hash) {
  struct crypt_data data;
  data.initialized = 0;
  strcpy(hash, crypt_r(cod, "aa", &data));
}


void *produtor(void *v){
  while (1) {
    //Bloqueia
    sem_wait(&pos_vazia);
    sem_wait(&bloqueio);

    //Entra na regiao Critica
    strcpy(buffer[final], senha);
    buffer[final][TAM_SENHA]   = '\0';
            
    incrementa_senha();
    final = (final + 1) % MAX;

    //Sai da regiao critica
    //Desbloqueia
    sem_post(&bloqueio);
    sem_post(&pos_ocupada);
  }
  return NULL;   
}

void *consumidor(void *v){
  char senhaGerada[TAM_SENHA+1];
  senhaGerada[TAM_SENHA]='\0';
  while(1){  
    //Bloqueia
    sem_wait(&pos_ocupada);
    sem_wait(&bloqueio);
    //Regiao Critica
    
    strcpy(senhaGerada, buffer[inicio]);
    inicio = (inicio +1) % MAX;
    //Saiu da regiao critica
    //Desbloqueia
    sem_post(&bloqueio);
    sem_post(&pos_vazia);

    testa_senha(senhaGerada);
  }
  return NULL;
}