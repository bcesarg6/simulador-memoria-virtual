/* Simulador de memória virtual            *
* Autor: Bruno Cesar, bcesar.g6@gmail.com  *
* Trabalho de SO                           *
* ---------------------------------------- *
* Tamanho da memória = 2^32                *
* Endereços de 32 bits:                    *
* ________________________________________ */

// 4278190080 bit_mask para pagina

#include "simulador.h"

/* Dados globais */
unsigned int n_frames;
unsigned int tam_pag;
unsigned char algoritmo;
unsigned int tam_tabela;
unsigned int n_instructions = 0;
unsigned int hbit = 0;
unsigned int faults = 0;
unsigned int hackaux = 0;

node* sentinela;
page_node* p_sentinela;
pagina* tabela;

/* Pega argumentos com getopt */
void getArgs(int argc, char *argv[]){
	extern char *optarg;
	char op;

	if(argc < 3){
		help();
	}

	struct option longopts[] = {
    {"frames", 1, NULL, 'f'},
    {"tamanho", 1, NULL, 't'},
    {"algoritmo", 1, NULL, 'a'}
	};

	while ((op = getopt_long(argc, argv, "f:t:a:h", longopts, NULL)) != -1) {
		switch (op) {
			case 'f':
				n_frames = atoi(optarg);
				break;

      case 't':
        tam_pag = atoi(optarg);
        break;

      case 'a':
        algoritmo = atoi(optarg);
        break;

			case 'h':
        help();
        break;
      }
    }
}

void printLista(){
  node* n = sentinela->prox;

  while(n != NULL){
    printInstruction(n->i);
    n = n->prox;
  }
}

/* Faz a leitura da entrada gerando uma lista de instructions */
void read(){
  char* str;
  char* token;

  unsigned int addr, type, value;
  instruction i;
  node* n = sentinela;

  str = (char*) malloc(40);

  scanf("%[^\n]", str);
  getchar();

  while(strcmp(str, "#eof") != 0){
    token = strtok(str, " ");

    token[strlen(token)-1] = 0; //remove o ":"
    i.addr = (int)strtol(token, NULL, 0);
    //printf("%u ", addr);

    token = strtok(NULL, " ");
    if(token[0] == 'W'){
      i.type = 1;
    } else{
      i.type = 0;
    }
    //printf("%u ", type);

    token = strtok(NULL, " ");
    i.value = (int)strtol(token, NULL, 0);
    //printf("%u\n", value);

    //printInstruction(i);
    node* new = malloc(sizeof(node));
    new->i = i;
    new->prox = NULL;
    n->prox = new;
    n = n->prox;

    n_instructions++;

    scanf("%[^\n]", str);
    getchar();
  }
}

unsigned int decode_endereco(unsigned int addr){
  unsigned int aux = addr;

  unsigned int shift = 32 - hbit;
  return aux >> shift;
}

unsigned int decode_deslocamento(unsigned int addr){
  unsigned int aux = addr;

  aux = addr << hbit;
  return aux >> hbit;
}

/* Configura a execução */
void config(){
  unsigned int i;

  tam_tabela = MEM_SIZE / (tam_pag * 1024); //tam_pag é em KB considerando 1KB = 1024 Bytes
  tabela = malloc(sizeof(pagina) * tam_tabela);

  /* Inicializa a tabela de paginas */
  for (i = 0; i < tam_tabela; i++) {
    tabela[i].frame = 0;
    tabela[i].bit = 0;
    tabela[i].referenciada = 0;
    tabela[i].modificada = 0;
  }

  /* Geração da highest bit para o endereçamento */
  unsigned int aux = tam_tabela;
  while(aux >>= 1)
    hbit++;

  //printf("hbit = %u\n",hbit);
  if(!algoritmo){
    time_t t;
    srand((unsigned) time(&t));
  }
}

/* Checa se os parametros são validos, aborta caso não sejam */
void checkData(){
	if (n_frames < 1){
		printf("Quantidade de frames invalida. Abortando...\n");
		help();
	}

  if (n_frames > 1073741824){
    printf("Memoria principal maior que 2^30 frames. Abortando...\n");
    help();
  }

  if (tam_pag < 1){
		printf("Tamanho da pagina invalido. Abortando...\n");
		help();
	}
}

void print_tabela(){
  int i;
  printf("\nNum\t| bit\t | ref\t | mod\n");

  for (i = tam_tabela - 1; i >= 0; i--) {
    printf("%d\t", i);
    printf("%u\t", tabela[i].bit);
    printf("%u\t", tabela[i].referenciada);
    printf("%u\t", tabela[i].modificada);
    printf("\n");
  }
  printf("----------------------\n");
}

/* Implementação do algoritmo NRU */
unsigned int algoritmo_nru(){
  int classe0, classe1, classe2, classe3, i, i0, i1, i2, i3;
  unsigned int* classe0v, *classe1v, *classe2v, *classe3v;
  classe1 = classe2 = classe3 = classe0 = i0 = i1 = i2 = i3 = 0;

  for (i = 0; i < tam_tabela; i++) {
    if(tabela[i].referenciada && tabela[i].modificada){
      classe3++;
    } else if(tabela[i].referenciada){
      classe2++;
    } else if(tabela[i].modificada){
      classe1++;
    } else{
      classe0++;
    }
  }

  classe0v = malloc(sizeof(unsigned int) * classe0);
  classe1v = malloc(sizeof(unsigned int) * classe1);
  classe2v = malloc(sizeof(unsigned int) * classe2);
  classe3v = malloc(sizeof(unsigned int) * classe3);

  for (i = 0; i < tam_tabela; i++) {
    if(tabela[i].referenciada && tabela[i].modificada){
      classe3v[i3] = i;
      i3++;
    } else if(tabela[i].referenciada){
      classe2v[i2] = i;
      i2++;
    } else if(tabela[i].modificada){
      classe1v[i1] = i;
      i1++;
    } else{
      classe0v[i0] = i;
      i0++;
    }
  }

  int r;

  if(i0 > 0){
    r = rand() % i0;
    r = classe0v[r];
  } else if(i1 > 0){
    r = rand() % i1;
    r = classe1v[r];
  } else if(i2 > 0){
    r = rand() % i2;
    r = classe2v[r];
  } else if(i3 > 0){
    r = rand() % i3;
    r = classe3v[r];
  }

  free(classe0v);
  free(classe1v);
  free(classe2v);
  free(classe3v);

  return r;
}

/* Implementa o algoritmo FIFO */
unsigned int algoritmo_fifo(){
  page_node* n = p_sentinela;
  unsigned int r;
  int i = 0;

  while(n->prox != NULL){
    n = n->prox;
    if(i == hackaux){
      r = n->numero;
      hackaux++;
    }
    i++;
  }

  return r;
}

/* Adiciona uma referencia a lista de pages FIFO */
void add_fifo(unsigned int num){
  //printf("Rolando um add fifo! %d\n", num);
  page_node* n = p_sentinela;

  while(n->prox != NULL){
    n = n->prox;
  }

  page_node* new = malloc(sizeof(page_node));
  new->numero = num;
  new->prox = NULL;

  n->prox = new;
}

/* Reseta o bit de referenciada da tabela de paginas */
void reset(){
  int i;
  for (i = 0; i < tam_tabela; i++) {
    tabela[i].referenciada = 0;
  }
}

/* Execução principal */
void run(){
  node* n = sentinela->prox;
  instruction i;
  unsigned int addr_dado;
  unsigned int endereco;
  unsigned int deslocamento;
  unsigned int num_ops = 0;
  unsigned int r_page;
  unsigned int paginas_carregadas = 0;

  while(n != NULL){
    if(!algoritmo) num_ops++; //NRU
    i = n->i;
    addr_dado = i.value;
    endereco = decode_endereco(addr_dado);
    deslocamento = decode_deslocamento(addr_dado);
    //printf("Endereco = %u | deslocamento = %u\n", endereco, deslocamento);

    if(tabela[endereco].bit == 0){

      if (paginas_carregadas > n_frames){
        //printf("Page fault! Pagina %u\n", endereco);
        faults++;
        if(algoritmo)
          r_page = algoritmo_fifo();
        else
          r_page = algoritmo_nru();

        tabela[r_page].bit = 0;
        tabela[r_page].referenciada = 0;
        tabela[r_page].modificada = 0;

        tabela[endereco].bit = 1;
        tabela[endereco].referenciada = 1;
        if(i.type) tabela[endereco].modificada = 1;

      } else{
        //printf("Carregando a página %u \n", endereco);
        paginas_carregadas++;
        if(algoritmo) add_fifo(endereco);
        tabela[endereco].bit = 1;
        tabela[endereco].referenciada = 1;
        if(i.type) tabela[endereco].modificada = 1;
      }

    } else{
      tabela[endereco].referenciada = 1;
      if(i.type) tabela[endereco].modificada = 1;
    }

    /* NRU */
    if(!algoritmo){
      if(num_ops > 10){
        num_ops = 0;
        reset();
      }
    }

    //print_tabela();
    n = n->prox;
  }

}

int main(int argc, char *argv[]){
	getArgs(argc, argv);
  checkData();
  config();

  sentinela = malloc(sizeof(node));
  p_sentinela = malloc(sizeof(page_node));
  read();

  printf("|    SIMULADOR DE MEMÓRIA VIRTUAL   |\n");
  printf("-------------------------------------\n");
  printf("| Tamanho da pagina:     %uKB\n", tam_pag);
  printf("| Total de páginas:      %u\n", tam_tabela);
  printf("| Numero de page frames: %u\n", n_frames);
  printf("| Total de instruções:   %u\n", n_instructions);
  printf("| Algoritmo escolhido: ");
  if(algoritmo == 0){
    printf("Não Usada Recentemente\n");
  } else{
    printf("FIFO\n");
  }
  printf("-------------------------------------\n");
  printf("         Iniciando simulação!        \n");

  //printLista();
  run();
  printf("Total de page faults: %u\n", faults);
  printf("--- FIM DA EXECUÇÃO! ---\n");
}
