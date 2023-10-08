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
    int val[9] = {1, 2, 0xFFFF, 4, 5, 6, 7, 8, 9};
    result = Write_multiple_regs("127.0.0.1", 502, 1, 9, val); // nao pode 
    printf("result = %d\n", result);
    //result = Read_h_regs("127.0.0.1", 502, 5, 2, val);;
    return 0; 
}
