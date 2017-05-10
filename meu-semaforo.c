#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>

long int PMrand();
void defineConjuntos();
void *monitor();
void *coletor();
void *visualizador();


//define a quantidade de valores que um sensor pode armazenar
#define NVALORES 100
typedef struct {
    long int valor;
    char* nome;
    long int valores[NVALORES];
}sensor;


//define as operacoes para serem realizadas
#define SOM 0
#define SUB 1
#define MUL 2
#define DIV 3
typedef struct{
    char* analise;
    sensor sensorUm;
    sensor sensorDois;
    int operacao;
}conjunto;

//define quantos turnos serão realizados
#define TURNOS 50
float record[TURNOS][NVALORES];
int turno;

//cria as estruturas de sensores e conjuntos
sensor sensores[5];
conjunto conjuntos[5];
int selecionado;

//semaforos
sem_t escrita, mostra, coleta;

//define os conjuntos de sensores
void defineConjuntos(){
    conjuntos[0].analise = "Ritmo Cardíaco";
    conjuntos[0].sensorUm = sensores[0];
    conjuntos[0].sensorDois = sensores[1];
    conjuntos[0].operacao = SOM;
    
    conjuntos[1].analise = "Suprimento Sanguineo";
    conjuntos[1].sensorUm = sensores[1];
    conjuntos[1].sensorDois = sensores[2];
    conjuntos[1].operacao = MUL;
    
    conjuntos[2].analise = "Oxigênio";
    conjuntos[2].sensorUm = sensores[2];
    conjuntos[2].sensorDois = sensores[3];
    conjuntos[2].operacao = DIV;
    
    conjuntos[3].analise = "Onda P";
    conjuntos[3].sensorUm = sensores[3];
    conjuntos[3].sensorDois = sensores[4];
    conjuntos[3].operacao = SUB;
    
    conjuntos[4].analise = "Repolarização";
    conjuntos[4].sensorUm = sensores[0];
    conjuntos[4].sensorDois = sensores[4];
    conjuntos[4].operacao = SOM;
}

void *monitor(){
    while(turno < TURNOS){
        sem_wait(&escrita);
        // sensores[0].nome = "sensor";
        // printf("%s\n", sensores[0].nome);
        // printf("monitora\n");
        int x;
        switch(conjuntos[selecionado].operacao){
            case SOM :
                for(x=0;x<NVALORES;x++)
                    record[turno][x] = conjuntos[selecionado].sensorUm.valores[x] + conjuntos[selecionado].sensorDois.valores[x];
                break;
            case SUB :
                for(x=0;x<NVALORES;x++)
                    record[turno][x] = conjuntos[selecionado].sensorUm.valores[x] - conjuntos[selecionado].sensorDois.valores[x];
                break;
            case MUL :
                for(x=0;x<NVALORES;x++)
                    record[turno][x] = conjuntos[selecionado].sensorUm.valores[x] * conjuntos[selecionado].sensorDois.valores[x];
                break;
            case DIV :
                for(x=0;x<NVALORES;x++){
                    // printf("%ld\n", conjuntos[selecionado].sensorUm.valores[x] / conjuntos[selecionado].sensorDois.valores[x]);
                    if(conjuntos[selecionado].sensorDois.valores[x] != 0){
                        record[turno][x] = (float)conjuntos[selecionado].sensorUm.valores[x] / (float)conjuntos[selecionado].sensorDois.valores[x];    
                    }else{
                        record[turno][x] = -1;
                    }
                }
                break;
        }
        // sleep(2);
        turno++;
        if(turno < TURNOS)
            sem_post(&coleta);
        else
            sem_post(&mostra);
    }
}

void *coletor(){
    turno = 0;
    while(turno < TURNOS){
        sem_wait(&coleta);
        // inicio = time(0);
        // diff = 0;
        // long int anterior = 0;
        int igual=0;
        selecionado = PMrand()%5;
        // printf("coleta\n");
        printf("Análise: %s\n",conjuntos[selecionado].analise);
        int scan;
        for(scan=0;scan<NVALORES;scan++){
            conjuntos[selecionado].sensorUm.valores[scan] = PMrand()%100;
            conjuntos[selecionado].sensorDois.valores[scan] = PMrand()%100;
        }
        // sleep(2);
        sem_post(&escrita);
    }
}

void *visualizador(){
        sem_wait(&mostra);
        // printf("mostra\n");
        int x,y;
        for(y=0;y<TURNOS;y++){
            printf("Turno %d\n", y);
            for(x=0;x<NVALORES;x++)
                printf("%d\t%.2f\n", x, record[y][x]);
        }
        exit(0);
        // sleep(2);
        // sem_post(&coleta);
}

#define a 16807
#define m 2147483647
#define q (m / a)
#define r (m % a)
static long int seed = 1;
long int PMrand()
{
    long int hi = seed / q;
    long int lo = seed % q;
    long int test = a * lo - r * hi;
    if(test > 0)
        seed = test;
    else    seed = test + m;
    return seed;
}


void main(){
    const int nthr = 3;
    pthread_t threads[nthr];
    
    defineConjuntos();
    
    sem_init(&escrita, 0, 0);
    sem_init(&mostra, 0, 0);
    sem_init(&coleta, 0, 1);
    
    pthread_create(&threads[1],NULL,coletor,NULL);
    pthread_create(&threads[0],NULL,monitor,NULL);
    pthread_create(&threads[2],NULL,visualizador,NULL);
    
    int x;
    for(x=0;x<nthr;x++)
    pthread_join(threads[x],NULL);
}