#include "ModbusTCP.h"
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>


int Write_multiple_regs(char* server_add, int port, int st_r, int n_r, int *val);
int Read_h_regs(char* server_add, int port, int st_r, int n_r, int *val);