#include <iostream>
#include <thread>	
#include <mutex>	
#include <condition_variable>	
#include <chrono>

using namespace std;

condition_variable cv; // Класс condition_variable – это примитив синхронизации, который может использоваться для блокировки потока или нескольких потоков до тех пор,
					   // пока другой поток не изменит общую переменную (не выполнит условие) и не уведомит об этом condition_variable.

mutex m;  // Mutex — это примитив синхронизации, предоставляющий эксклюзивный доступ к общему ресурсу только одному потоку.

bool completed = false;
int  number = 0;

void sent() // функция, отправляющая данные в поток
{
	while (true) 
	{	
		// unique_lock позволяет управлять блокировкой и разблокировкой mutex
		unique_lock<mutex> lk(m); // блокировка потока для изменения переменных

		number++; // увеличение переменной number на 1 для удобного отслеживания исходящих и входящих данных в потоке
		cout << "Sent: " << number << endl; // отправка данных в поток

		completed = true;

		// Ручная разблокировка выполняется перед уведомлением,
		// чтобы не разбудить ожидающий поток только для повторной блокировки

		lk.unlock(); // ручная разблокировка потока
		cv.notify_one(); // notify_one уведомляет conditional_variable об изменении состояния потока  

		this_thread::sleep_for(chrono::milliseconds(1000)); // Операция ожидания освобождает мьютекс и приостанавливают выполнение потока. В нашем случае на 1 секунду
	}
}
void recieved() // функция, принимающая данные из потока
{
	while (true) 
	{
		// unique_lock позволяет управлять блокировкой и разблокировкой mutex
		unique_lock<mutex> lk(m); // блокировка потока для изменения переменных

		cv.wait(lk, [] {return completed; }); // ждем, пока sent() отправит данные
		cout << "Recieved: " << number << endl;
		completed = false;

		lk.unlock(); // ручная разблокировка потока
		cv.notify_one(); // notify_one уведомляет conditional_variable об изменении состояния потока
	}
}

int main()
{
	thread thread1(sent);
	thread thread2(recieved);

	thread1.join();
	thread2.join();

	return 0;
}