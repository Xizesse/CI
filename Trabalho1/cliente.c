#include "ModbusTCP.h"
#include "ModbusAP.h"
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>

int unit_id = 3;

//Avaliação : Exame teórico + Exame prático
//2 turnos práticos e um turno teórico no bloco B


//TODO Respostas direito
//TODO a outra função

//IP
//porta
//unit id -> variavel global

//Exceções
//parte mais dificil é o time out, fica para o fim
//e depois ainda disto, o loop

// gcc -c ModbusTCP.c ModbusAP.c cliente.c
// gcc -o cliente cliente.o ModbusAP.o ModbusTCP.o


int main () {
    int result;
    int a;
    int val[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int response[10] = {0};
    result = Write_multiple_regs("127.0.0.1", 502, 1, 9, val); 
    printf("result = %d\n", result);
    printf("\n\n\n");

    result = Read_h_regs("127.0.0.1", 502, 1, 9, response);
    printf("result = %d\n", result);
    for (int i = 0; i < 10; i++)
    {
        printf("Register %d = %d\n", i+1, response[i]);
    }
    return 0; 
}
