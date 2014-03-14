/*
*
*	En la terminal: ./client <nombre del archivo>
*
*/

#include <stdio.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	int udpSocket;
	struct sockaddr_in udpServer, udpClient;	//Creamos la estructura para el cliente
	socklen_t addrlen = sizeof(udpClient),len;
	int status;
	char buffer[255] = "";
	int i;
	int option;
	int fd;
	int contador=0;
	long start, end;
	char ip[17];
	u_short clientPort;
	char *tok;
	int fileSize;
	int totalReadBytes, readBytes;
	char fileBuffer[10240];
	int writeBytes;

	udpSocket = socket(AF_INET, SOCK_DGRAM, 0); //Crear socket

	udpServer.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&udpServer.sin_addr.s_addr);
	udpServer.sin_port = htons(8080);

	status = sendto(udpSocket,argv[1],strlen(argv[1]),0,(struct sockaddr*)&udpServer, sizeof(udpServer));
	bzero(buffer,255);
	status = recvfrom(udpSocket, buffer, 255, 0, (struct sockaddr*)&udpClient, &addrlen );

	inet_ntop(AF_INET,&(udpClient.sin_addr),ip,INET_ADDRSTRLEN);
	clientPort = ntohs(udpClient.sin_port);

	printf("Archivo: %s de [%s:%i]\n\r",buffer,ip,clientPort); //Se reciben los parametros para conectarnos con el server
	
	status = sendto(udpSocket,"OK",strlen("OK"),0,(struct sockaddr*)&udpServer, sizeof(udpServer)); 
	totalReadBytes = 0;
	while(totalReadBytes < fileSize) {
		readBytes = recvfrom(udpSocket,fileBuffer,10240,0,(struct sockaddr*)&udpClient, &addrlen );
		
	while(writeBytes < readBytes) {
		writeBytes += write(fd,fileBuffer+writeBytes,readBytes-writeBytes);
	}
	
	totalReadBytes += writeBytes;
	}
	close(fd); 

	bzero(buffer,255);
	status = recvfrom(udpSocket, buffer, 255, 0, (struct sockaddr*)&udpClient, &addrlen );
	sleep(5);	
	printf("\n\tArchivo exitosamente recibido del servidor [%s:%i]\n\n",ip,clientPort,buffer);
	close(udpSocket);

	return 0;
}