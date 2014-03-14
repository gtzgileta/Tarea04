/*
*
*	En la terminal: ./server
*
*/

#include <stdio.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) { 
	int udpSocket, clientSocket;
    	struct sockaddr_in udpServer, udpClient;
	int status;
    	socklen_t addrlen = sizeof(udpClient);
	char buffer[255];
	char requestFileName[255];
	char ip[17];
	u_short clientPort;
	int fd,bytes;
	char filebuffer[10240];
	int contador = 0;
	int file;
	struct stat filestat;

	long start,end;
	int totalSendBytes,readBytes,sendBytes;

	printf("\n\tEsperando solicitud del cliente..\n\n");
	
	sleep(2);

	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(udpSocket == -1) {
		fprintf(stderr,"Can't create UDP Socket");
		return 1;
	}
	

   	 udpServer.sin_family = AF_INET;
   	 inet_pton(AF_INET,"0.0.0.0",&udpServer.sin_addr.s_addr);
   	 udpServer.sin_port = htons(8080);

    	status = bind(udpSocket, (struct sockaddr*)&udpServer,sizeof(udpServer));

    	if(status != 0) {
		fprintf(stderr,"Can't bind");
    	}

	bzero(requestFileName,255);

       
	status = recvfrom(udpSocket, requestFileName, 255, 0, (struct sockaddr*)&udpClient, &addrlen); 
	

	inet_ntop(AF_INET,&(udpClient.sin_addr),ip,INET_ADDRSTRLEN);
	clientPort = ntohs(udpClient.sin_port);
	printf("El cliente [%s:%i] quiere el archivo: '%s'\n",ip,clientPort,requestFileName);

	file = open(requestFileName,O_RDONLY);
	if(file == -1) {
		fprintf(stderr,"Archivo %s no existe\n",requestFileName);  
		sendto(udpSocket, "El archivo no existe\n\r", strlen("El archivo no existe"),0,(struct sockaddr*)&udpClient,sizeof(udpClient));
		return -1;
	}
  
	fstat(file,&filestat);

	sprintf(buffer,"\n\tArchivo encontrado y listo para transferencia",filestat.st_size);
	sendto(udpSocket,buffer,strlen(buffer),0,(struct sockaddr*)&udpClient,sizeof(udpClient));
	bzero(buffer,255);
	status = recvfrom(udpSocket, buffer, 255, 0, (struct sockaddr*)&udpClient, &addrlen );
    
    while(totalSendBytes < filestat.st_size) {  //Se transfiere el archivo
	readBytes = read(file,filebuffer,10240);
	sendBytes = 0;
	while(sendBytes < readBytes) {
		sendBytes += sendto(udpSocket,filebuffer+sendBytes,readBytes-sendBytes,0,(struct sockaddr*)&udpClient,sizeof(udpClient));
	}
	totalSendBytes += sendBytes;
    }
     
	printf("\nEnviando.............\n\r");
	
	sleep(5);
	sendto(udpSocket, "\nListo!", strlen("\nListo!"),0,(struct sockaddr*)&udpClient,sizeof(udpClient));
		
	sleep(2);
	printf("\nArchivo enviado!\n\n\r"); 
	close(udpSocket);
	return 0;
}