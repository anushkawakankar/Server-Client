// client code for UDP socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define IP_PROTOCOL 0
#define IP_ADDRESS "127.0.0.1" // localhost
#define PORT_NO 15050
#define NET_BUF_SIZE 10000
#define cipherKey 'S'
#define sendrecvflag 0

// funtion to clear buffer
void remove_listall()
{
	struct dirent *d2;
	DIR *dr2 = opendir(".");
	while((d2 = readdir(dr2)) != NULL)
	{
		if(strncmp(d2->d_name,"listall",7) == 0)
		{
			remove("listall");
			break;
		}
	}
	closedir(dr2);
}
void list_dir()
{
	struct dirent *de;
	DIR *dr = opendir("../Server");

	if (dr == NULL)
	{
			printf("Could not open Server directory" );
	}

	while ((de = readdir(dr)) != NULL)
		if(strncmp(de->d_name,"server.c",8) != 0 && strncmp(de->d_name,"a.out",5) != 0 )
					printf("%s\n", de->d_name);

	closedir(dr);

}
void clearBuf(char* b)
{
	int i;
	for (i = 0; i < NET_BUF_SIZE; i++)
		b[i] = '\0';
}

// function for decryption
char Cipher(char ch)
{
	return ch ^ cipherKey;
}

// function to receive file
int recvFile(char* buf, int s,int fPtr)
{
	// FILE* fPtr = fopen(buf,"w+");
	int i;
	char ch;
	for (i = 0; i < s; i++) {
		ch = buf[i];
		// File *fPtr = fopen("output.txt", "w+");
		// printf("%s\n",buf);
		ch = Cipher(ch);
		if (ch == EOF)
		{
			return 1;
		}
		else
		{
			// printf("%c", ch);
			write(fPtr,&ch,1);
			// fwrite(&ch,sizeof(ch),1,fPtr);
		}
	}


	// fclose(fPtr);
	close(fPtr);
	return 0;
}

// driver code
int main()
{
	int sockfd, nBytes;
	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con);
	addr_con.sin_family = AF_INET;
	addr_con.sin_port = htons(PORT_NO);
	addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	char net_buf[32];
	FILE* fp;

	// socket()
	sockfd = socket(AF_INET, SOCK_DGRAM,
					IP_PROTOCOL);

	if (sockfd < 0)
		printf("\nfile descriptor not received!!\n");
	else
		printf("\nfile descriptor %d received\n", sockfd);

	while (1) {
		printf("\nPlease enter file name to receive:\n");
		scanf("%s", net_buf);
		if(strncmp(net_buf, "listall",7) == 0)
			list_dir();
		// printf("%s\n",net_buf);
		// FILE* fPtr = fopen(net_buf, "w+");

		int	fPtr =  open(net_buf, O_CREAT | O_WRONLY | O_APPEND, 0644);
		// if (fPtr == NULL)
		// 	printf("fail\n");
		// else
		// 		printf("ban gaya\n");
		// printf("%d\n",fPtr);
		sendto(sockfd, net_buf, NET_BUF_SIZE,
			sendrecvflag, (struct sockaddr*)&addr_con,
			addrlen);

		printf("\n---------Data Received---------\n");

		while (1) {
			// receive
			clearBuf(net_buf);
			nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE,
							sendrecvflag, (struct sockaddr*)&addr_con,
							&addrlen);
			// printf("hel\n");

			// process
			if (recvFile(net_buf, NET_BUF_SIZE,fPtr)) {
				break;
			}
			// fclose(fPtr);
		}
		remove_listall();
		printf("\n-------------------------------\n");
	}
	return 0;
}
