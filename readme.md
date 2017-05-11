# Trabalho 1 - Sistema Operacionais
## Eletrocardiograma


### Sensores
```c
#define NVALORES 100
typedef struct {
    long int valores[NVALORES];
}sensor;
```

### Conjunto de Análise
```c
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
```

### Estruturas para controle da execução

```c
//define quantos turnos serão realizados
#define TURNOS 50
float record[TURNOS][NVALORES];
int turno;
```

### Semaforos
```c
sem_t escrita, mostra, coleta;
```

### Módulo Coletor

```c
void *coletor(){
    turno = 0;
    while(turno < TURNOS){
        sem_wait(&coleta);
        selecionado = PMrand()%5;
        printf("Análise: %s\n",conjuntos[selecionado].analise);
        int scan;
        for(scan=0;scan<NVALORES;scan++){
            conjuntos[selecionado].sensorUm.valores[scan] = PMrand()%100;
            conjuntos[selecionado].sensorDois.valores[scan] = PMrand()%100;
        }
        sem_post(&escrita);
    }
}
```

### Módulo Monitor
```c
void *monitor(){
    while(turno < TURNOS){
        sem_wait(&escrita);
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
                if(conjuntos[selecionado].sensorDois.valores[x] != 0){
                        record[turno][x] = (float)conjuntos[selecionado].sensorUm.valores[x] / (float)conjuntos[selecionado].sensorDois.valores[x];    
                    }else{
                        record[turno][x] = -1;
                    }
                }
                break;
        }
        turno++;
        if(turno < TURNOS)
            sem_post(&coleta);
        else
            sem_post(&mostra);
    }
}
```

### Módulo Visualizador

```c
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
}
```

### Main

```c
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
```