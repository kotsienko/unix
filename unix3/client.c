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
	struct sockaddr_in address; // struct sockaddr_in используется для обработки адреса сетевого взаимодействия
	int result;
	char ch[80];

	sockfd = socket(AF_INET, SOCK_STREAM, 0); // Функция socket создает сокет, привязанный к определенному поставщику транспортных услуг

											  // AF_INET - семейство адресов интернет-протокола версии 4 (IPv4).
 
											  // SOCK_STREAM - тип сокета, который обеспечивает последовательные, надежные, двусторонние потоки байтов на основе соединения
											  // с механизмом передачи данных OOB. Этот тип сокета использует протокол управления передачей (TCP)
											  // для семейства интернет-адресов (AF_INET или AF_INET6).

	printf("Enter your message: \n");

	address.sin_family = AF_INET; // sin_family - address family
	address.sin_addr.s_addr = inet_addr("127.0.0.1"); // sin_addr.s_addr - 32 bit IP address

													  // inet_addr () - преобразовывает строку IP в целочисленное значение порядка байтов в сети, 
													  // которое используется для sockaddr_in.sin_addr.s_addr.

													  // IP address "127.0.0.1" - локальный IP-адрес (ЭВМ, на которой происходит запуск)

	address.sin_port = htons(10000); // 16 bit TCP/UDP port number
									 // htons () - преобразовывает номер порта из порядка байтов хоста в целочисленное значение порядка байтов сети. (хост к сети)

	len = sizeof(address); // получение длины в байтах структуры sockaddr_in
	result = connect(sockfd, (struct sockaddr *)&address, len);	// Функция connect устанавливает соединение с указанным сокетом.

																// sockfd - дескриптор, идентифицирующий неподключенный сокет.

																// (struct sockaddr *)&address - указатель на структуру sockaddr, с которой должно быть установлено соединение.

																// len - длина в байтах структуры sockaddr, на которую указывает параметр (struct sockaddr *)&address.
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