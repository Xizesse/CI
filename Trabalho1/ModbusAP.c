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

    //---------- checking consistency of input
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
    //se tiver erro, o APDU_R[0] vai ser 0x90
    
    #ifdef DEBUG
    //print the apdu_r in a line
    if(APDU_R[0] == 0x90)
    {
        printf("Detetou erro do servidor\n");
        printf("APDU_R: ");
        for (int i = 0; i < 2; i++)
        {
            printf("%02X ", APDU_R[i]);
        }
    }
    else
    {
        printf("APDU_R: ");
        for (int i = 0; i < 5; i++)
        {
            printf("%02X ", APDU_R[i]);
        }
        printf("\n");
    }
    #endif

    if (result < 0)
    {   
        #ifdef DEBUG
        printf("Error sending request\n");
        #endif
        return -1;
    }
    else if (APDU_R[0] != 16)
    {
        #ifdef DEBUG
        printf("Error in response\n");
        #endif
        return -1;
    }
    

    //------------returns the number of written registers or negative if error

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
    //check starting register
    //está zero porque já decrementei
    if (st_r < 0 || st_r > 65535)
    {
        printf("Invalid starting register\n");
        return -1;
    }

    //check number of registers
    //Addressable Elements acording to MODBUSchapter 1-123

    if (n_r < 0 || n_r > 125) //para o read já é 125
    {
        printf("Invalid number of registers\n");
        return -1;
    }
    

    //!------------------------build APDU
    #ifdef DEBUG
    printf("Building APDU\n");
    #endif
    
    APDU[0] = 0x03;	//function code - 16 = write multiple registers
    //Start address
    APDU[1] = st_r >> 8 & 0xFF;
    APDU[2] = st_r & 0xFF;
    //num reg
    APDU[3] = n_r >> 8 & 0xFF;
    APDU[4] = n_r & 0xFF;
    //byte count
    
    APDUlen = 5;
    APDU_Rlen = 3 + 2*n_r;

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

    
    //!---------- checking the response
    //se tiver erro, o APDU_R[0] vai ser 0x83
    
    if (result < 0)
    {   
        #ifdef DEBUG
        printf("Error sending request\n");
        #endif
        return -1;
    }
    else if (APDU_R[0] != 3)
    {
        #ifdef DEBUG
        printf("Returned Wrong FC\n");
        #endif
        return -1;
    }

    #ifdef DEBUG
    //print the apdu_r in a line
    printf("APDU_R: ");
    for (int i = 0; i < APDU_Rlen; i++)
    {
        printf("%02X ", APDU_R[i]);
    }
    printf("\n");
    #endif
    
    //!---------- copy the values to the array
    for (int i = 0; i < n_r; i++)
    {
        val[i] = APDU_R[3 + 2*i] << 8 | APDU_R[4 + 2*i];
    }
    
    


    //------------returns the number of read registers or negative if error

    return APDU_R[2];

}






