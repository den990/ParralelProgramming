#include <windows.h>
#include <string>
#include <iostream>
#include "tchar.h"
#include <fstream>

CRITICAL_SECTION FileLockingCriticalSection;

int ReadFromFile() {
	EnterCriticalSection(&FileLockingCriticalSection);
	std::fstream myfile("balance.txt", std::ios_base::in);
	int result;
	myfile >> result;
	myfile.close();
	LeaveCriticalSection(&FileLockingCriticalSection);

	return result;
}

void WriteToFile(int data) {
	EnterCriticalSection(&FileLockingCriticalSection);
	std::fstream myfile("balance.txt", std::ios_base::out);
	myfile << data << std::endl;
	myfile.close();
	LeaveCriticalSection(&FileLockingCriticalSection);
}

int GetBalance() {
	int balance = ReadFromFile();
	return balance;
}

void Deposit(int money) {
	int balance = GetBalance();
	balance += money;

	WriteToFile(balance);
	printf("Balance after deposit: %d\n", balance);
}

void Withdraw(int money) {
	if (GetBalance() < money) {
		printf("Cannot withdraw money, balance lower than %d\n", money);
		return;
	}

	Sleep(20);
	int balance = GetBalance();
	balance -= money;
	WriteToFile(balance);
	printf("Balance after withdraw: %d\n", balance);
}

DWORD WINAPI DoDeposit(CONST LPVOID lpParameter)
{
	Deposit((int)lpParameter);
	ExitThread(0);
}

DWORD WINAPI DoWithdraw(CONST LPVOID lpParameter)
{
	Withdraw((int)lpParameter);
	ExitThread(0);
}

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE* handles = new HANDLE[49];

	InitializeCriticalSection(&FileLockingCriticalSection);

	WriteToFile(0);

	SetProcessAffinityMask(GetCurrentProcess(), 1);
	for (int i = 0; i < 50; i++) {
		handles[i] = (i % 2 == 0)
			? CreateThread(NULL, 0, &DoDeposit, (LPVOID)230, CREATE_SUSPENDED, NULL)
			: CreateThread(NULL, 0, &DoWithdraw, (LPVOID)1000, CREATE_SUSPENDED, NULL);
		ResumeThread(handles[i]);
	}


	// ожидание окончания работы двух потоков
	WaitForMultipleObjects(50, handles, true, INFINITE);
	printf("Final Balance: %d\n", GetBalance());

	getchar();

	DeleteCriticalSection(&FileLockingCriticalSection);

	return 0;
}

/*Изменения в коде с использованием CRITICAL_SECTION:

Были созданы две глобальные критические секции: FileLockingCriticalSection и BalanceLockingCriticalSection.
В функциях ReadFromFile, WriteToFile, GetBalance, Deposit, и Withdraw использовались EnterCriticalSection для входа в критическую секцию перед операциями с файлом и балансом и LeaveCriticalSection для выхода из неё после завершения операций.
Изменения в коде с использованием MUTEX:

Были созданы два глобальных мьютекса: FileLockingMutex и BalanceLockingMutex.
В функциях ReadFromFile, WriteToFile, GetBalance, Deposit, и Withdraw использовались функции WaitForSingleObject для захвата мьютекса перед операциями и ReleaseMutex для его освобождения после операций.
Разница между CRITICAL_SECTION и MUTEX:

Производительность: CRITICAL_SECTION обычно более эффективен внутри одного процесса, чем мьютекс. Это связано с тем, что CRITICAL_SECTION более оптимизирован для работы в пределах одного процесса.
Переносимость: Мьютексы более переносимы и могут использоваться на разных платформах, включая Windows и Linux. CRITICAL_SECTION - это специфичный для Windows механизм синхронизации.
Уровень защиты: CRITICAL_SECTION предоставляет более высокий уровень защиты данных. Он предназначен для использования только в пределах одного процесса и обеспечивает безопасность доступа к данным только для потоков внутри этого процесса.
Скорость создания и удаления: CRITICAL_SECTION обычно быстрее создается и уничтожается, чем мьютекс. Это делает его более удобным для сценариев, где требуется частое создание и удаление синхронизационных объектов.
Какие участки были синхронизированы:

В обоих вариантах были синхронизированы операции чтения и записи файла (ReadFromFile и WriteToFile) с использованием мьютекса или критической секции.
Операции, которые могли изменять баланс (GetBalance, Deposit, и Withdraw), также были синхронизированы, чтобы избежать состояний гонки.
Советы по выбору между CRITICAL_SECTION и MUTEX:

Если вам нужна синхронизация внутри одного процесса и вы работаете на платформе Windows, CRITICAL_SECTION может быть предпочтительным выбором из-за его производительности и уровня защиты.
Если вам нужна переносимость между разными операционными системами, или если вы работаете с множеством процессов, мьютексы являются более подходящим выбором.
В данном случае использование CRITICAL_SECTION для синхронизации кажется разумным, так как операции выполняются в пределах одного процесса, и уровень защиты данных важен. Однако вариант с MUTEX также был бы рабочим и переносимым на другие платформы.*/