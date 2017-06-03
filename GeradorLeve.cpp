#include <stdio.h>
#include <stdlib.h>

#define TMT 2
#define AMOUNT 100
#define N 16
#define TOTAL 10000

#define LEVE 1
#define INTENSA 2

#define CURTO 0
#define LONGO 1

int main(){
    FILE *proclist;
    int carga = INTENSA;
    float longo = 1.5*TMT, curto = TMT/2, duration=0;
    int CPUNumber;
    float time=0;
    int i;

    switch(carga){
    case LEVE:
        int i;
        duration=(float)TMT/((float)AMOUNT/3);
        printf("duration=%f\n",duration);
        proclist = fopen("ListaLeve.txt", "w");
        for(i=0;i<TOTAL;i++){
            CPUNumber = rand()%N;
            fprintf(proclist,"%d ",CPUNumber);
            fprintf(proclist,"%f ",time);
            time=time+duration;
            switch(i%2){
            case CURTO:
                fprintf(proclist,"%f\n",curto);
                break;
            case LONGO:
                fprintf(proclist,"%f\n",longo);
                break;
            }
        }
        break;
    case INTENSA:
        duration=(float)TMT/((float)AMOUNT);
        printf("duration=%f\n",duration);
        proclist = fopen("ListaIntensa.txt", "w");
        for(i=0;i<TOTAL;i++){
            CPUNumber = rand()%N;
            fprintf(proclist,"%d ",CPUNumber);
            fprintf(proclist,"%f ",time);
            time=time+duration;
            switch(i%2){
            case CURTO:
                fprintf(proclist,"%f\n",curto);
                break;
            case LONGO:
                fprintf(proclist,"%f\n",longo);
                break;
            }
        }
        break;

    }
    fclose(proclist);
    return 0;
}
