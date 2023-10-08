//A header file contains
   //Function protoypes
   //Type definitions
   //Macro definitions
   //Conditional copilation directives

#include <stdint.h>

int Send_Modbus_request(char* server_add, int port, uint8_t* APDU, int APDUlen, char* APDU_R);
