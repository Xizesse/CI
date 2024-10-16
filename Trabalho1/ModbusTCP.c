#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <errno.h>

#include "ModbusTCP.h"

#define BUF_LEN 256
//#define DEBUG

int Send_Modbus_request(char* server_add, int port, uint8_t* APDU, int APDUlen, char* APDU_R)
{
    #ifdef DEBUG
    printf("\t\tSend_Modbus_request:\n");
    #endif

    

	uint8_t PDU[256];
	uint8_t PDU_R[256];
    int PDUlen = 0;   
    int PDU_Rlen = 0;
    int APDU_Rlen = 0;
    int sent_bytes = 0;
    int recv_bytes = 0;
    static int TI = 0; //transaction identifier
    extern int unit_id; //unit identifier
    
    //!-----------------------generate TI
    #ifdef DEBUG
    printf("Generating TI\n ");
    #endif
    TI++;
    

    //!------------------------assempble APDU
    #ifdef DEBUG
    printf("Assembling APDU\n ");
    #endif

    PDU[0] = (TI >> 8) & 0xFF; // TI high byte
    PDU[1] = TI & 0xFF; // TI low byte

    PDU[2] = 0; // Protocol ID high byte
    PDU[3] = 0; // Protocol ID low byte

    PDU[4] = ((1+APDUlen)>>8) & 0xFF; // Lenght high byte
    PDU[5] = (1+APDUlen) & 0xFF; // Lenght low byte

    PDU[6] = unit_id; //definido como global no cliente

    //!------------------------copy APDU to MBAPDU
    for (int i = 0; i < APDUlen; i++)
    {
        PDU[7 + i] = APDU[i];
    }

    PDUlen = 7 + APDUlen;

    //!------------------------open TCP client socket and connects to server
    #ifdef DEBUG
    printf("Opening TCP client socket and connecting to server\n ");
    #endif


    int sock, len;
    struct sockaddr_in serv;
    socklen_t addlen = sizeof(serv); 
    
    sock = socket(PF_INET, SOCK_STREAM, 0);
    
    if (sock < 0) {
        #ifdef DEBUG
        perror("socket");
        #endif
        close (sock);
        return -50;
    }
    else {
        #ifdef DEBUG
        printf("socket opened\n");
        #endif
    }

    serv.sin_family = AF_INET; 
    serv.sin_port = htons(port); //converts port to network byte order
    inet_aton(server_add, &serv.sin_addr); //converte str to binary address in network byte order
    
    
    //!------------------------connects to server

    struct timeval timeout;
    timeout.tv_sec = 3; //seconds
    timeout.tv_usec = 15; //microseconds
    //setsockopt is a function that sets options for a socket
    //SO_RCVTIMEO sets the timeout value for receiving data for the socket
    //SOL_SOCKET is the level aw which the option is defined

    if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
        #ifdef DEBUG
        perror("setsockopt failed\n");
        #endif
        close (sock);
        return -50;
    }

    #ifdef DEBUG
    //print the address connecting to 
    printf("Connecting to %s\n", server_add);
    #endif

    if(connect(sock, (struct sockaddr *) &serv, addlen) < 0) {
        #ifdef DEBUG
        perror("connect");
        #endif
        close (sock);
        return -50;
    }
    else {
        #ifdef DEBUG
        printf("connected to server\n");
        #endif
    }

    //!------------------------sends PDU to server
    sent_bytes = send(sock, PDU, PDUlen, 0);
    if (sent_bytes< 0) { 
        #ifdef DEBUG
        perror("send");
        #endif
        close (sock);
        return -50;
    }
    else {
        #ifdef DEBUG
        printf("sent %d bytes\n", sent_bytes);
        #endif
    }
    #ifdef DEBUG
    printf("PDU = ");
    for (int i = 0; i < PDUlen; i++)
    {
        printf("%02X ", PDU[i]);
    }
    printf("\n");
    #endif


    //!-----------------reads response from server
     
    //TODO passar isto para um loop (mais para o fim)
    recv_bytes = recv(sock, PDU_R, BUF_LEN, 0);
    
    if (recv_bytes < 0) {
        #ifdef DEBUG
        if (errno == EAGAIN || errno == EWOULDBLOCK) 
            printf("timeout occurred\n");
        
        else
            perror("recv");
        #endif
        return -50;
       }
    else
        #ifdef DEBUG
        printf("received %d bytes\n", recv_bytes);
        #endif
    

    PDU_Rlen = recv_bytes;
    //buffer de leitura
    //do while ?

    #ifdef DEBUG
    printf("PDU_R = ");
    for (int i = 0; i < recv_bytes; i++)
    {
        printf("%02X ", PDU_R[i]);
    }
    printf("\n");
    #endif

    //!------------------------if response, remove MBAP and PDU_R -> APDU_R
    //00 01(TI) 00 00() 00 06 00 10 00 00 00 05
    //2 bytes TI
    //2 bytes Protocol ID
    //2 bytes Lenght
    //1 byte Unit ID

    //1 byte function code
    //2 bytes start address
    //2 bytes num reg
    

    for (int i = 0; i < PDU_Rlen; i++)
    {
        APDU_R[i] = (uint8_t)PDU_R[7 + i];
    }
    APDU_Rlen = PDU_Rlen - 7;

    #ifdef DEBUG
    printf("APDU_Rlen = %d\n", APDU_Rlen);
    //print response in a line
    printf("APDU_R = ");
    for (int i = 0; i < APDU_Rlen; i++)
    {
        printf("%02X ", APDU_R[i]);
    }
    printf("\n");
    #endif

    
    //!------------------------closes TCP client socket with server
    close(sock);
    //!------------------------returns APDU_R and 0-ok or -1-error(timeout)
    return 0;
}

