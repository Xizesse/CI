#include "ModbusTCP.h"
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>





#define DEBUG
#define BUF_LEN 256

//gcc -c ModbusTCP.c
//gcc -c ModbusAP.c
//gcc -o ModbusAP ModbusAP.o ModbusTCP.o
//./ModbusAP 


int Write_multiple_regs(char* server_add, int port, int st_r, int n_r, int *val)
{
    uint8_t APDU[BUF_LEN];
    int APDUlen = 0;
    uint8_t APDU_R[BUF_LEN];
    int APDU_Rlen = 0;
    st_r --; // para bater certo com o que eu fiz para a baixo disto

    //---------- checking consistency of input
    //TODO dependendo do que é o ultimo, ver se o final fica bem depois de
    //TODO 2^16 ou 123
    //check starting register
    //está zero porque já decrementei
    if (st_r < 0 || st_r > 65535)
    {
        printf("Invalid starting register\n");
        return -1;
    }

    //check number of registers
    //Addressable Elements acording to MODBUSchapter 1-123

    if (n_r < 0 || n_r > 123)
    {
        printf("Invalid number of registers\n");
        return -1;
    }
    //values for 2byte registers (65535 = 0xFFFF)
    for (int i = 0; i < n_r; i++)
    {
        if (val[i] < 0 || val[i] > 65535)
        {
            printf("Invalid values\n");
            return -1;
        }
    }
    

    //------------------------build APDU
    #ifdef DEBUG
    printf("Building APDU\n");
    #endif
    
    APDU[0] = 16;	//function code - 16 = write multiple registers
    //Start address
    APDU[1] = st_r >> 8 & 0xFF;
    APDU[2] = st_r & 0xFF;
    //num reg
    APDU[3] = n_r >> 8 & 0xFF;
    APDU[4] = n_r & 0xFF;
    //byte count
    APDU[5] = 2*n_r;
    //data
    for (int i = 0; i < n_r; i++)
    {
        APDU[6 + 2*i] = val[i] >> 8 & 0xFF;
        APDU[7 + 2*i] = val[i] & 0xFF;
    }
    
    APDUlen = 6 + 2*n_r;


    #ifdef DEBUG
    printf("APDU BUILT\n");
    //print the apdu in a line
    for (int i = 0; i < APDUlen; i++)
    {
        printf("%02X ", APDU[i]);
    }
    printf("\n");
    #endif

    int result = Send_Modbus_request("127.0.0.1", 502, APDU, APDUlen, APDU_R);

    //---------- checking the response
    
    if (result < 0)
    {
        printf("Error sending request\n");
        return -1;
    }

    

    //Cop

    //------------returns the number of written registers or negative if error

    return 0;
}




