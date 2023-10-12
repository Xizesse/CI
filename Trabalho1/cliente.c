#include "ModbusTCP.h"
#include "ModbusAP.h"
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>

int unit_id = 51;

//Avaliação : Exame teórico + Exame prático
//2 turnos práticos e um turno teórico no bloco B


//IP
//porta
//unit id -> variavel global

//Exceções
//parte mais dificil é o time out, fica para o fim
//e depois ainda disto, o loop

// gcc -c ModbusTCP.c ModbusAP.c cliente.c
// gcc -o cliente cliente.o ModbusAP.o ModbusTCP.o
int resultToX(int result)
{
    int X;
    if (result == -50)
    {return -1;} //nãO mb
    else if (result < 0 )
    {return -result;}
    else
    return 0;
}

int main () {
    int result;
    int X;
    int a;
    //int val[9] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    //int val[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int val[15] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ,11 , 12 ,13 ,14 ,15};

    int response[10] = {0};
    //char *server_add = "10.227.145.228";
    //char * server_add = "10.227.103.44";
    char * server_add = "127.0.0.1";
    result = Write_multiple_regs(server_add, 502, 1, 15, val); 
    //printf("result = %d\n", result);
    printf("result = %d\n", resultToX(result));
    printf("\n\n\n");

    result = Write_multiple_regs(server_add, 502, 1, 9, val); 
    //printf("result = %d\n", result);
    printf("result = %d\n", resultToX(result));
    printf("\n\n\n");


    result = Read_h_regs(server_add, 502, 1, 9, response);
    //printf("result = %d\n", result);
    printf("result = %d\n", resultToX(result));

    for (int i = 0; i < 10; i++)
    {
        printf("Register %d = %d\n", i+1, response[i]);
    }
    return 0; 
}
