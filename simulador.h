/* Simulador de memória virtual            *
* Autor: Bruno Cesar, bcesar.g6@gmail.com  *
* Trabalho de SO                           */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <limits.h>
#include <time.h>

typedef struct instruction{
  unsigned int addr;  // Endereço
  unsigned char type;  // Tipo da operação
  unsigned int value; // Valor
}instruction;

typedef struct node{
  instruction i;
  struct node* prox;
}node;

typedef struct page_node{
  unsigned int numero;
  struct page_node* prox;
}page_node;

typedef struct pagina{
  unsigned int frame;
  unsigned char bit; //Presente/Ausente
  unsigned char referenciada;
  unsigned char modificada;
}pagina;

#define MEM_SIZE 4294967296 //2^32 (unsigned)

/* Printa uma instruction */
void printInstruction(instruction i){
  printf("%u ", i.addr);

  if(i.type == 0){
    printf("R ");
  } else{
    printf("W ");
  }

  printf("%u\n", i.value);
}

/* Exibe ajuda e finaliza o programa */
void help(){
	printf("\nAJUDA - Simulador memoria virtual\n");
  printf("\nParametros:\n");
  printf("  f : Numero de page frames          | f >= 1\n");
  printf("  t : Tamanho do frame em KB         | t >= 1\n");
  printf("  a : Algoritmo para substição de página\n\t 0 para NRU, 1 para FIFO\n");

  printf("\nModo de uso:\n");
  printf("./simulador <Params> < [entrada.txt]\n");
	exit(1);
}
