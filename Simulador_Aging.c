/************************************************
Autor: Ryan Dias Fernandes
UNIFESP SJC
************************************************/


#include <stdio.h>
#include <strings.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define MAXMV 10
#define MAXMR 5
#define MAXMT 5
#define MAXIDADE 65536



/*estruturas*/
typedef struct{
    int quadro; /*qual o quadro real onde está a página*/
    int enderecoV; /*posição na area de swap*/
    int presenca; /*indica se a página está em uso*/
    int permicao[3]; /*cada posição determina permição de leitura, escrita e execução*/
    int mod; /*representa se a página foi modificada*/
    int referencia; /*representa se a página foi referenciada*/
    int idade; /*representa a idade da página*/
    char dado;
}tPage;

/*Variaveis*/
    tPage MemoriaReal[5];
    tPage MemoriaVirtual[10];
    tPage MemoriaTemp[5];

    int Page_Fault = 0;


/*funções*/
void inicia_memoriaR(){
    /*preencher totalmente a memoria real*/
    for(int i = 0; i < MAXMR; i++){
        MemoriaVirtual[i].presenca = 1;
        MemoriaVirtual[i].quadro = i;
        MemoriaReal[i] = MemoriaVirtual[i];
    }
}
void inicia_memoriaT(){
    /*preencher totalmente a memoria real*/
    for(int i = 0; i < MAXMT; i++) MemoriaTemp[i] = MemoriaVirtual[i+MAXMR];
}
void inicia_memoriaV(){
    /*preencher totalmente a memoria virtual*/
    for(int i = 0; i < MAXMV; i++){
        MemoriaVirtual[i].quadro = -1;
        MemoriaVirtual[i].enderecoV = i;
        MemoriaVirtual[i].presenca = 0;
        MemoriaVirtual[i].permicao[0] = 1;
        MemoriaVirtual[i].permicao[1] = 0;
        MemoriaVirtual[i].permicao[2] = 0;
        MemoriaVirtual[i].mod = 0;
        MemoriaVirtual[i].referencia = 0;
        MemoriaVirtual[i].idade = 0;
        MemoriaVirtual[i].dado = 65+i;
    }
}

void aging(){
    for(int i = 0; i < MAXMV; i++){
        MemoriaVirtual[i].idade = (MemoriaVirtual[i].idade >> 1);
        if(MemoriaVirtual[i].referencia == 1) MemoriaVirtual[i].idade = MemoriaVirtual[i].idade | (1 << 15);
    }
}

void refenciar(){

    printf("As referencias foram:\n");

    for(int i = 0; i < MAXMV; i++){
        MemoriaVirtual[i].referencia = rand() % 2;
        printf("%c:%d ",MemoriaVirtual[i].dado, MemoriaVirtual[i].referencia);
    }
    printf("\n\n");
}

void substituir(int pag){

    int menor_idade = MAXIDADE, j = 0, quadro = 0;

    for(int i = 0; i < MAXMV; i++){
        if(MemoriaVirtual[i].presenca == 1){
            if(MemoriaVirtual[i].idade < menor_idade){
                menor_idade = MemoriaVirtual[i].idade;
                quadro = MemoriaVirtual[i].quadro;
                j = i;
            }
        }
    }

    tPage pag_temp;
    for(int i = 0; i < MAXMT; i++){
        if(MemoriaTemp[i].enderecoV == MemoriaVirtual[pag].enderecoV){
            MemoriaVirtual[j].presenca = 0;
            MemoriaVirtual[j].quadro = -1;
            pag_temp = MemoriaTemp[i];
            MemoriaTemp[i] = MemoriaReal[quadro];
            MemoriaTemp[i].presenca = 0;
            MemoriaTemp[i].quadro = -1;
        }
    }
    MemoriaReal[quadro] = pag_temp;
    MemoriaReal[quadro].quadro = quadro;
    MemoriaReal[quadro].presenca = 1;
    MemoriaVirtual[pag].quadro = quadro;
    MemoriaVirtual[pag].presenca = 1;

}

void enquadrar(){

    int Page_Fault_A = 0;

    for(int i = 0; i < MAXMV; i++){
        if(MemoriaVirtual[i].referencia == 1 && MemoriaVirtual[i].presenca == 0){
            Page_Fault++;
            Page_Fault_A++;
            substituir(i);
        }
    }
    printf("Nesse tick houve(ram) %d falta(s) de pagina\n\n", Page_Fault_A);
}

void mostra_idade(){

    for(int i = 0; i < MAXMV; i++) printf("Pagina %c: %d\n", MemoriaVirtual[i].dado, MemoriaVirtual[i].idade);

}

void tick(){

    refenciar();
    aging();
    enquadrar();

}

int main(int argc, char **argv){

    inicia_memoriaV();
    inicia_memoriaR();
    inicia_memoriaT();

    int op;

    printf("Simulacao iniciada\n\n");
    printf("O dados nos quadros sao:\n");
    printf("|");
    for(int i = 0; i < 5; i++){
        printf(" %c |", MemoriaReal[i].dado);
    }
    printf("\n");
    printf("1 - Novo tique de relogio\n");
    printf("2 - Reset\n");
    printf("3 - Mostrar idade das paginas em quadro\n");
    printf("4 - executar n tiques\n");

    while(1){
        srand(time(NULL));
        scanf("%d", &op);

        system("cls"); //usar em sistema windows
        //system("clear"); //usar em sistema linux

        switch(op){

        case 1:
            tick();
        break;
        case 2:
            Page_Fault = 0;
            inicia_memoriaV();
            inicia_memoriaR();
            inicia_memoriaT();
        break;
        case 3:
            mostra_idade();
        break;
        case 4:
          printf("Quantos tiques devem ser executados?\n");
          int tiques;
          scanf("%d", &tiques);
          for(int i = 0; i < tiques; i++) tick();
        break;
        }
        printf("Os dados nos quadros sao:\n");
        printf("|");
        for(int i = 0; i < MAXMR; i++){
            printf(" %c |", MemoriaReal[i].dado);
        }
        printf("\nOs dados nas paginas sao:\n");
        printf("|");
        for(int i = 0; i < MAXMR; i++){
            printf(" %c |", MemoriaTemp[i].dado);
        }
        printf("\n\n");
        printf("No total houve(ram) %d falta(s) de pagina\n\n", Page_Fault);
        printf("1 - Novo tique de relogio\n");
        printf("2 - Reset\n");
        printf("3 - Mostrar idade das paginas em quadro\n");
        printf("4 - executar n tiques\n");
    }

    return 0;
}
