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
    st_r --; // para bater certo com o que eu fiz para a baixo disto

    //!---------- checking consistency of input
    //TODO check address

    //hmm check port
    if (port < 0 || port > 65535)
    {
        #ifdef DEBUG
        printf("Invalid port\n");
        #endif
        return -1;
    }


    //hmm check starting register
    //está zero porque já decrementei
    if (st_r < 0 || st_r > 65535)
    {
        #ifdef DEBUG
        printf("Invalid starting register\n");
        #endif
        return -1;
    }

    //hmm check number of registers
    //Addressable Elements acording to MODBUSchapter 1-123 (Writing)

    if (n_r < 0 || n_r > 123)
    {
        #ifdef DEBUG
        printf("Invalid number of registers\n");
        #endif
        return -1;
    }
    //hmm values for 2byte registers (65535 = 0xFFFF)
    for (int i = 0; i < n_r; i++)
    {
        if (val[i] < 0 || val[i] > 65535)
        {
            #ifdef DEBUG
            printf("Invalid values\n");
            #endif
            return -1;
        }
    }
    

    //!------------------------build APDU
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
    for (int i = 0; i < APDUlen; i++)
    {
        printf("%02X ", APDU[i]);
    }
    printf("\n");
    #endif

    int result = Send_Modbus_request(server_add, port, APDU, APDUlen, APDU_R);


    //!---------- checking the response
    //se tiver erro do Modbus, o APDU_R[0] vai ser 0x90
    
    if (result < 0)
    {   
        #ifdef DEBUG
        printf("Modbus internal error\n");
        
        #endif
        return -50;
    }
    else if (APDU_R[0] == APDU[0] + 0x80) //se receber o de erro
    {
        #ifdef DEBUG
        printf("Exception code\n");
        #endif
        if (APDU_R[1] == 1)
        return -1;
        else if (APDU_R[1] == 2)
        return -2;
        else if (APDU_R[1] == 3)
        return -3;
        else if (APDU_R[1] == 4)
        return -4;
        else return -50;
    }

    //!------------returns the number of written registers or negative if error

    return APDU_R[3] << 8 | APDU_R[4];

}


int Read_h_regs(char* server_add, int port, int st_r, int n_r, int *val)
{
    uint8_t APDU[BUF_LEN];
    uint8_t APDU_R[BUF_LEN];

    int APDUlen = 0;
    int APDU_Rlen = 0;
    st_r --; // para bater certo com o que eu fiz para a baixo disto

    //!---------- checking consistency of input
    //TODO check address

    //hmm check port
    if (port < 0 || port > 65535)
    {
        #ifdef DEBUG
        printf("Invalid port\n");
        #endif
        return -1;
    }

    //hmm check starting register
    //está zero porque já decrementei
    if (st_r < 0 || st_r > 65535)
    {
        #ifdef DEBUG
        printf("Invalid starting register\n");
        #endif
        return -1;
    }

    //hmm check number of registers
    //Addressable Elements acording to MODBUSchapter 1-125 (Read)

    if (n_r < 0 || n_r > 125) //para o read já é 125
    {
        #ifdef DEBUG
        printf("Invalid number of registers\n");
        #endif
        return -1;
    }
    

    //!------------------------build APDU
    #ifdef DEBUG
    printf("Building APDU\n");
    #endif
    
    APDU[0] = 0x03;	//function code - 16 = write multiple registers
    //hmm Start address
    APDU[1] = st_r >> 8 & 0xFF;
    APDU[2] = st_r & 0xFF;
    //hmm num reg
    APDU[3] = n_r >> 8 & 0xFF;
    APDU[4] = n_r & 0xFF;
    //hmm byte count
    APDUlen = 5;
    APDU_Rlen = 3 + 2*n_r;

    #ifdef DEBUG
    printf("APDU BUILT\n");
    for (int i = 0; i < APDUlen; i++)
    {
        printf("%02X ", APDU[i]);
    }
    printf("\n");
    #endif

    //! ------- Send_Modbus_request
    int result = Send_Modbus_request(server_add, port, APDU, APDUlen, APDU_R);

    
    //!---------- checking the response
    
    if (result < 0)
    {   
        #ifdef DEBUG
        printf("Modbus internal error\n");
        #endif
        return -50;
    }
    else if (APDU_R[0] == APDU[0] + 0x80) //se receber o de erro
    {
        #ifdef DEBUG
        printf("Exception code\n");
        #endif
        if (APDU_R[1] == 1)
        return -1;
        else if (APDU_R[1] == 2)
        return -2;
        else if (APDU_R[1] == 3)
        return -3;
        else if (APDU_R[1] == 4)
        return -4;
        else return -50;
    }
    //!---------- copy the values to the array
    for (int i = 0; i < n_r; i++)
    {
        val[i] = APDU_R[2 + 2*i] << 8 | APDU_R[3 + 2*i];
    }
    

    //!------------returns the number of read registers or negative if error

    return APDU_R[2];

}






