#include <sys/types.h>
#include <sys/socket.h> // содержит определения флагов уровня сокета
#include <stdio.h>
#include <netinet/in.h> // Protocols are specified
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t wasSigHup = 0; //Целочисленный тип, к которому можно получить доступ как к атомарному объекту даже при наличии асинхронных прерываний, создаваемых сигналами.	

//Объявление обработчика сигнала
void sigHupHandler(int r)
{
	wasSigHup = 1;
}

int main()
{
	int listenfd = 0;

	// Создание сокета
	// Функция socket() создает конечную точку для связи и возвращает дескриптор файла для сокета.
	// Принимает 3 аргумента domain, который определяет семейство протоколов созданного сокета; тип; протокол, значение 0 может использоваться для выбора протокола по умолчанию из выбранного домена и типа.
	// Функция возвращает значение -1, если произошла ошибка. В противном случае он возвращает целое число, представляющее вновь назначенный дескриптор.

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	if(listenfd == -1){	
		perror("socket"); // Выводит текстовое описание кода ошибки, хранящегося в данный момент в системной переменной errno, в stderr.
		return -1;
	}
	

	// Задает параметр сокета
	// Принимает дескриптор, который идентифицирует сокет; 
			// уровень, на котором определяется опция;
			// параметр сокета, для которого должно быть установлено значение, параметр optname должен быть параметром сокета, определенным в пределах указанного уровня, иначе поведение не определено;
			// указатель на буфер, в котором указано значение для запрошенной опции;
			// Размер в байтах буфера, на который указывает параметр optval
			
	// SO_REUSEADDR указывает, что правила проверки адресов, передаваемых с помощью вызова bind(2), должны позволять повторное использование локального адреса.

	int yes = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror ("setsockopt"); // Выводит текстовое описание кода ошибки, хранящегося в данный момент в системной переменной errno, в stderr.
		return -1;
	}


	// Устанавливает первые n байтов области, начинающейся с s в нули (пустые байты).  
	struct sockaddr_in addr;
	bzero(&addr, sizeof(struct sockaddr_in));		
	addr.sin_family = AF_INET;	
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");	
	addr.sin_port = htons(10000);	

	if (bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {	
		perror("bind");
		close(listenfd);
		return -1;
	}
	 
	// Если задано значение SOMAXCONN, базовый поставщик услуг, ответственный за сокет s, установит для отставания максимально разумное значение
	if (listen(listenfd, SOMAXCONN) < 0) {	
		perror("listen");
		return -1;
	}


	// Регистрация обработчика событий
	struct sigaction sa;
	sigaction(SIGHUP, NULL, &sa);
	sa.sa_handler = sigHupHandler;
	sa.sa_flags |= SA_RESTART;
	sigaction(SIGHUP, &sa, NULL);	

	// Блокировка сигнала
	sigset_t blockedMask, origMask;	
	sigemptyset(&blockedMask);
	sigaddset(&blockedMask, SIGHUP);
	sigprocmask(SIG_BLOCK, &blockedMask, &origMask);

	char ch[80];
	int client_fd, res;
	int numberOfClients=0;

	
	while (!wasSigHup)	
	{

		memset(ch, 0, sizeof(ch));	
		printf("---------------\n");
		printf("server waiting\n");

		int maxFd = -1;	

		fd_set fds;	
		FD_ZERO(&fds);	
		FD_SET(listenfd, &fds);	


		// Подготовка списка фд
		if(listenfd>maxFd) maxFd = listenfd;
		if(numberOfClients>0){
			FD_SET(client_fd, &fds);
			if(client_fd > maxFd) maxFd = client_fd;
		}

		// n на единицу больше самого большого номера описателей из всех наборов.
		/* Вызов функции pselect(), которая временно
           разблокирует необходимый сигнал и дождётся одного
           из трёх событий */

		res = pselect (maxFd + 1, &fds, NULL, NULL, NULL, &origMask);	
		
		if(res == -1){
			if (errno == EINTR){	
				puts("Caught kill signal");
				return -1;
			}
		}

		if(res>=0)
		{
		    //Установка сигнал
			if (FD_ISSET(listenfd, &fds))	
			{
				
				int client_sockfd = accept(listenfd, NULL, NULL);		
				if(client_sockfd == 0) perror("accept client_sockfd");

				if(numberOfClients>=1){
					close(client_sockfd);
					continue;
				}
				if(client_fd>=0){
					client_fd = client_sockfd;
					numberOfClients++;
					printf("Client accepted\n");
					printf("Num of clients %i accepted\n\n", numberOfClients);
				}
				else perror("accept");
				continue;
			}

             //Новые данные
			if (FD_ISSET(client_fd, &fds))
			{
				read(client_fd, &ch, 80);
				int k = strlen(ch);
				printf("Message from client: ");
				for(int i=0; i<k; i++) printf("%c", ch[i]);

				if(k>0){
					send(client_fd, ch, 80, 0);
					printf("\nsize = %d\n", k);
					k=0;
				}
				else{
					close(client_fd);
					printf("Connection closed\n");
					client_fd=-1;
					numberOfClients--;
				}
			}
		}
	}
}