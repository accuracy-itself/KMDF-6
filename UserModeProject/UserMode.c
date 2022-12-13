#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

int main(int argc, char* argv[])
{
	HANDLE hFile;
	DWORD fileSize, bytesRead, dwWritten;
	OVERLAPPED osWrite = { 0 };
	wchar_t* buff;
	while (1) {
		hFile = CreateFile(L"R:\\pproc.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			printf("Invalid handle\n");
			printf(GetLastError());
		}
		
		fileSize = GetFileSize(hFile, NULL);
		//fileSize = 2;
		buff = (char*)malloc(fileSize + 1);
		ZeroMemory(buff, sizeof(buff));
		ReadFile(hFile, (LPVOID)buff, fileSize, &bytesRead, (LPOVERLAPPED)NULL);
		buff[fileSize] = '\0';
		if (strcmp(buff, "z") == 0) { //сигнал для создания процесса
			TCHAR czCommandLine[] = L"calc";
			STARTUPINFO siCalc = { sizeof(siCalc) };
			PROCESS_INFORMATION piCalc;
			if (CreateProcess(NULL, czCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &siCalc, &piCalc)) {
				CloseHandle(hFile);
				HANDLE outFile = CreateFile(L"R:\\pproc.txt", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				DWORD processPid = piCalc.dwProcessId;
				DWORD tempPid = processPid;
				int counter = 0;
				while (tempPid != 0) {
					tempPid /= 10;
					++counter;
				}
				wchar_t *buffToWrite;
				buffToWrite = calloc(sizeof(wchar_t), (counter + 1) * 3);
				//_itoa_s(processPid, buffToWrite, counter + 1, 10);
				swprintf_s(buffToWrite, sizeof(wchar_t) * (counter + 1), L"%d", processPid);
				buffToWrite[counter] = '\0';
				printf("Buff: %s", buffToWrite);
				WriteFile(outFile, (LPCVOID)buffToWrite, sizeof(buffToWrite), &dwWritten, NULL);
				CloseHandle(outFile);
				Sleep(10);
				return 0;
			}
		}
		realloc(buff, sizeof(buff));
		
		CloseHandle(hFile);
		Sleep(10);
	}
	return 0;
}