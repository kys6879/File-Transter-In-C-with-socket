#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <stdio.h>
#include <unistd.h>
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h> // O_CREATIO, _O_RDONLY 사용하기 위해
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h> // _S_IWRITE 사용하기 위해

#define PORT	5555
#define IP		"127.0.0.1"
#define MAXBUF	1024

int main(int argc, char **argv){
	int client_sockfd;
	struct sockaddr_in addr;
	char file_name[1024];
	char buffer[1024];
	int dest_fd;
	int recv_len;
	int len;
	int totaln = 0;
	client_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	
	if(client_sockfd == -1){
		perror("socket error :");
		exit(0);
	}
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(IP);
	
	if (connect(client_sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		perror("fail to connect\n");
		return 1;
	}

	printf("File Name ? : ");
	scanf("%s",file_name);
	len = strlen(file_name);
	send(client_sockfd, file_name, len, 0);
	// 파일생성
	dest_fd = open(file_name, O_CREAT | O_EXCL | O_WRONLY,0666);
	if(dest_fd == -1){
		perror("file Create / Open Error");
		exit(1);
	}
	while(1){
		memset(buffer,0x00,MAXBUF);
		recv_len = recv(client_sockfd,buffer,MAXBUF,0);
		printf("recv_len : %d\n",recv_len);
		if(recv_len == 0) {
			printf("finish file !\n");
			break;
		}
		totaln += write(dest_fd,buffer,recv_len);
	}
	close(dest_fd);
	close(client_sockfd);
	
	return 0;
}
