#include <windows.h>
#include <string>
#include <iostream>

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{

    printf("Potok:  %d\n", *(int*)lpParam);
    //std::cout << std::to_string((int)lpParam) + "\n";

    ExitThread(0);
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    int N;
    std::cin >> N;

    HANDLE* handles = new HANDLE[N];

    for (int i = 0; i < N; i++)
    {
        int* threadIndex = new int(i);
        handles[i] = CreateThread(NULL, 0, &ThreadProc, &i, CREATE_SUSPENDED, NULL);

    }

    for (int j = 0; j < N; j++)
    {
        ResumeThread(handles[j]);
    }

    WaitForMultipleObjects(N, handles, true, INFINITE);


    return 0;
}