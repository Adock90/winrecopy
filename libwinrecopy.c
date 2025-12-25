#ifndef LIBWINRECOPY_H
#define LIBWINRECOPY_H

#include <Windows.h>
#include <stdio.h>

void CopyDirRecursive(char* DirFrom, char* DirTo)
{
	strcat_s(DirFrom, sizeof(DirFrom), "\\*");
	WIN32_FIND_DATA GetDirData;
	HANDLE FindDataHandle = FindFirstFile(DirFrom, &GetDirData);
	if (FindDataHandle == INVALID_HANDLE_VALUE)
	{
		printf("[WINRECOPY] Failed to retrive a valid file handle value. ERROR CODE: %d\n", GetLastError());
		exit(EXIT_FAILURE);
	}

	while (FindNextFile(FindDataHandle, &GetDirData))
	{
		if (GetDirData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			BOOL CopyDir = CreateDirectory(GetDirData.cFileName, NULL);
			if (!CopyDir)
			{
				printf("[WINRECOPY] Failed to get create directory %s. ERROR CODE: %d\n", GetDirData.cFileName, GetLastError());
				FindClose(FindDataHandle);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			LPVOID VerifyContentSRCBuff[1024];
			LPVOID VerifyContentDSTBuff[1024];

			WCHAR SrcBuff[1024];
			WCHAR DstBuff[1024];
			swprintf(SrcBuff, 1024, L"%s\\%s", DirFrom, GetDirData.cFileName);
			swprintf(DstBuff, 1024, L"%s\\%s", DirTo, GetDirData.cFileName);

			HANDLE VerifySourceFileHandle = CreateFile(
				SrcBuff,
				GENERIC_READ,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL
			);
			if (VerifySourceFileHandle == INVALID_HANDLE_VALUE)
			{
				wprintf(L"[WINRECOPY] Failed to open file: %ls. ERROR CODE: %d\n", VerifySourceFileHandle, GetLastError());
				CloseHandle(VerifySourceFileHandle);
				FindClose(FindDataHandle);
				exit(EXIT_FAILURE);
			}
			
			DWORD VerifyFileSize = GetFileSize(VerifySourceFileHandle, NULL);
			if (VerifyFileSize == 0)
			{
				wprintf(L"[WINRECOPY] Failed to get file size: %ls. ERROR CODE: %d\n", VerifySourceFileHandle, GetLastError());
				CloseHandle(VerifySourceFileHandle);
				FindClose(FindDataHandle);
				exit(EXIT_FAILURE);
			}

			BOOL VerifySourceFileContents = ReadFile(VerifySourceFileHandle, VerifyContentSRCBuff, 1024, 1024, NULL);
			if (!VerifySourceFileContents)
			{
				wprintf(L"[WINRECOPY] Failed to retrieve contents: %ls. ERROR CODE: %d\n", SrcBuff, GetLastError());
				CloseHandle(VerifySourceFileHandle);
				FindClose(FindDataHandle);
				exit(EXIT_FAILURE);
			}

			CloseHandle(VerifySourceFileHandle);

			BOOL CopyF = CopyFile(SrcBuff, DstBuff, TRUE);
			if (!CopyF)
			{
				wprintf(L"[WINRECOPY] Failed to copy file %ls to %ls.", SrcBuff, DstBuff);
				printf(" ERROR CODE: %d\n", GetLastError());
				FindClose(FindDataHandle);
				exit(EXIT_FAILURE);
			}
			else
			{
				wprintf(L"Copied %d bytes from %ls -> %ls\n", VerifyFileSize, SrcBuff, DstBuff);
			}

			HANDLE VerifyProductFileHandle = CreateFile(
				DstBuff,
				GENERIC_READ,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL
			);
			if (VerifyProductFileHandle == INVALID_HANDLE_VALUE)
			{
				wprintf(L"[WINRECOPY] Failed to open file: %ls. ERROR CODE: %d", DstBuff, GetLastError());
				FindClose(FindDataHandle);
				exit(EXIT_FAILURE);
			}

			DWORD VerifyProductFileSize = GetFileSize(VerifyProductFileHandle, NULL);
			if (VerifyProductFileSize == 0)
			{
				wprintf(L"[WINRECOPY] Failed to get file size: %ls. ERROR CODE: %d\n", DstBuff, GetLastError());
				CloseHandle(VerifyProductFileHandle);
				FindClose(FindDataHandle);
				exit(EXIT_FAILURE);
			}
			
			BOOL VerifyProductFileContents = ReadFile(VerifyProductFileHandle, VerifyContentDSTBuff, 1024, 1024, NULL);
			if (!VerifyProductFileContents)
			{
				wprintf(L"[WINRECOPY] Failed to retrieve contents: %ls. ERROR CODE: %d\n", DstBuff, GetLastError());
				CloseHandle(VerifyProductFileHandle);
				FindClose(FindDataHandle);
				exit(EXIT_FAILURE);
			}

			CloseHandle(VerifyProductFileHandle);

			if (VerifyFileSize != VerifyProductFileSize || VerifyContentSRCBuff != VerifyContentSRCBuff)
			{
				wprintf(L"[WINRECOPY] Failed to verify file content of: %ls:%ls\n", SrcBuff, DstBuff);
				FindClose(FindDataHandle);
				exit(EXIT_FAILURE);
			}
		}
	}
	FindClose(FindDataHandle);
}

#endif