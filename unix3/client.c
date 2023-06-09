#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int sockfd;
	int len;
	struct sockaddr_in address; // struct sockaddr_in ������������ ��� ��������� ������ �������� ��������������
	int result;
	char ch[80];

	sockfd = socket(AF_INET, SOCK_STREAM, 0); // ������� socket ������� �����, ����������� � ������������� ���������� ������������ �����

											  // AF_INET - ��������� ������� ��������-��������� ������ 4 (IPv4).
 
											  // SOCK_STREAM - ��� ������, ������� ������������ ����������������, ��������, ������������ ������ ������ �� ������ ����������
											  // � ���������� �������� ������ OOB. ���� ��� ������ ���������� �������� ���������� ��������� (TCP)
											  // ��� ��������� ��������-������� (AF_INET ��� AF_INET6).

	printf("Enter your message: \n");

	address.sin_family = AF_INET; // sin_family - address family
	address.sin_addr.s_addr = inet_addr("127.0.0.1"); // sin_addr.s_addr - 32 bit IP address

													  // inet_addr () - ��������������� ������ IP � ������������� �������� ������� ������ � ����, 
													  // ������� ������������ ��� sockaddr_in.sin_addr.s_addr.

													  // IP address "127.0.0.1" - ��������� IP-����� (���, �� ������� ���������� ������)

	address.sin_port = htons(10000); // 16 bit TCP/UDP port number
									 // htons () - ��������������� ����� ����� �� ������� ������ ����� � ������������� �������� ������� ������ ����. (���� � ����)

	len = sizeof(address); // ��������� ����� � ������ ��������� sockaddr_in
	result = connect(sockfd, (struct sockaddr *)&address, len);	// ������� connect ������������� ���������� � ��������� �������.

																// sockfd - ����������, ���������������� �������������� �����.

																// (struct sockaddr *)&address - ��������� �� ��������� sockaddr, � ������� ������ ���� ����������� ����������.

																// len - ����� � ������ ��������� sockaddr, �� ������� ��������� �������� (struct sockaddr *)&address.
	if (result < 0)	
	{
		perror("connect");
		exit(1);
	}

	while(1){
		gets(ch);
		write(sockfd, &ch, 80);
		read(sockfd, &ch, 80);
		if(!strcmp(ch, "-1")) break;
	}
	
	close(sockfd);
	puts("Connection closed");
	exit(0);
}