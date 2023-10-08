#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MODBUS_PORT 502

int main () {
	int sock, len;
	struct sockaddr_in serv;

	socklen_t addlen = sizeof(serv);
	int MBAPDU_len = 0;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	
	serv.sin_family = AF_INET;
	serv.sin_port = htons(MODBUS_PORT);	
	inet_aton("127.0.0.1",&serv.sin_addr);

	if (connect(sock, (struct sockaddr *) &serv, addlen) < 0){
		printf("error connecting\n");
		return -1;
	}
	
			//Header -> It is always 7 bytes long

	//Transaction Identifier 2 bytes
	MBAPDU[0] = 0;
	MBAPDU[1] = 1;
	
	//Protocol Identifier currently always has the value 0 2 bytes 
	MBAPDU[2] = 0;
	MBAPDU[3] = 0;
	 
	//lenght 2 bytes
	MBAPDU[4] = 0;
	MBAPDU[5] = 1+8; //unit_ID + APDU
	
	//unit_ID 1 byte
	MBAPDU[6] = 0;
	
				//APDU 

	MBAPDU[7] = 16;	//function code - 16 = write multiple registers
	//Start address
	MBAPDU[8] = 0;	
	MBAPDU[9] = 0;
	//num reg
	MBAPDU[10] = 0;	
	MBAPDU[11] = 1;	//only writing to one reg
	//num bytes
	MBAPDU[12] = 2;
	//reg val
	MBAPDU[13] = 33;	
	MBAPDU[14] = 55;	
	
	MBAPDU_len = 15;
	
	printf("MBAPDU:\n");
	for(int i= 0; i < MBAPDU_len ; i++){
		printf("%d ",MBAPDU[i]);
	}
	printf("\n");
		
	if (len = write(sock,MBAPDU, MBAPDU_len) < 0){
		printf("Error sending\n");
		return -1;
	}
	
	printf("packet sent\n");
	
	len = recv(sock, MBAPDU, 12, 0);
	if (len < 0 ){
		printf("Error receiving\n");
		return -1;
	}
	
	printf("MBAPDU de resposta:\n");
	for(int i= 0; i < len ; i++){
		printf("%d ",MBAPDU[i]);
	}
	
	close(sock);
}