#include <sys/socket.h> 
#include <sys/stat.h> 
#include <arpa/inet.h> 
#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <stdlib.h>

#define PORT 5555
#define MAXBUF 1024

int main(int argc , char **argv){
	int server_sockfd, client_sockfd; // 소켓
	int client_len, n;
	int file_read_len;
	char buf[MAXBUF];
	char file_name[MAXBUF];
	struct sockaddr_in clientaddr, serveraddr;
	int source_fd; // 파일 번호
	int chk_bind; // 연결 확인 변수
	int chk_write;
	int read_len;

	client_len = sizeof(clientaddr);

	/* socket() */
	server_sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(server_sockfd == -1) {
		perror("socket error : ");
		exit(0);
	}

	bzero(&serveraddr, sizeof(serveraddr)); // 해당 메모리 영역 초기화
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORT);

	chk_bind = bind(server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if(chk_bind > 0) {
		perror("bind error : ");
		exit(0);
	}

	/* listen() */
	if(listen(server_sockfd, 5)) {
		perror("listen error : ");
	}

		memset(buf, 0x00, MAXBUF);

		/* accept() */
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr, &client_len);
		printf("New Client Connect: %s\n", inet_ntoa(clientaddr.sin_addr));
		
		/* 파일 명 받기 */
		read_len = recv(client_sockfd,buf,MAXBUF,0);
		if(read_len > 0 ){
			memset(file_name,0x00,MAXBUF);
			strcpy(file_name,buf);
			printf("%s > %s \n", inet_ntoa(clientaddr.sin_addr),file_name);
		} else {
			close(client_sockfd);
			return 1;
		}
		/* 파일 열기 */
		source_fd = open(file_name,O_RDONLY);
		printf("source_fd : %d\n",source_fd);
		if(source_fd == -1){
			perror("file open error : ");
			return 1;
		}

		printf("before while \n");

		while(1){
			memset(buf, 0x00, MAXBUF);
			file_read_len = read(source_fd,buf,MAXBUF);
			printf("buf : %s",buf);
			printf("file_name : %s\n",file_name);
			printf("read_len : %d \n",file_read_len);
			chk_write = send(client_sockfd,buf,MAXBUF,0);
			if(file_read_len == EOF | file_read_len == 0) { 
				printf("finish file\n");
				break;
			}
		}
		close(client_sockfd);
		close(source_fd);
	close(server_sockfd);
	return 0;
}
