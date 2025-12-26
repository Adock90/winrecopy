#ifndef LIBWINRECOPY_H
#define LIBWINRECOPY_H

#include <Windows.h>
#include <stdio.h>

BOOLEAN CreateDirRecur(char* path)
{
	char TempStore[MAX_PATH];
	strcpy_s(TempStore, sizeof(TempStore), path);
	size_t Length = strlen(TempStore);
	if (TempStore[Length - 1] == '\\')
	{
		TempStore[Length - 1] = 0;
	}

	for (char* p = TempStore + 1; *p; p++)
	{
		if (*p == '\\')
		{
			*p = 0;
			CreateDirectoryA(TempStore, NULL);
			*p = '\\';
		}
	}
	return CreateDirectoryA(TempStore, NULL);
}


void CheckDirExists(char* Path, BOOL Correct)
{
	DWORD CheckResult = GetFileAttributesA(Path);
	if (CheckResult & FILE_ATTRIBUTE_DIRECTORY)
	{
		printf("Verified %s\n", Path);
	}
	else
	{
		if (Correct)
		{
			printf("Fixing %s\n", Path);
			if (!CreateDirectoryA(Path, NULL))
			{
				printf("[WINRECOPY] Failed to create %s. ERROR CODE: %d\n", Path, GetLastError());
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("[WINRECOPY] Failed to verify: %s\n", Path);
			exit(EXIT_FAILURE);
		}
	}
}

void CopyDirRecursive(char* DirFrom, char* DirTo)
{
	

	const char* illegalChars = "<>:\/\\|?*";

	char SrcBuff[MAX_PATH];
	char DstBuff[MAX_PATH];

	size_t Length = strlen(DirTo);
	char DirToClean[MAX_PATH];
	strcpy_s(DirToClean, sizeof(DirToClean), DirTo);
	if (DirToClean[Length - 1] == '\\')
	{
		DirToClean[Length - 1] = 0;
	}

	char SrcPath[MAX_PATH];
	sprintf_s(SrcPath, sizeof(SrcPath), "%s\\*", DirFrom);
	WIN32_FIND_DATA GetDirData;
	HANDLE FindDataHandle = FindFirstFileA(SrcPath, &GetDirData);
	if (FindDataHandle == INVALID_HANDLE_VALUE)
	{
		
		printf("[WINRECOPY] %s Failed to retrive a valid file handle value. ERROR CODE: %d\n", SrcPath, GetLastError());
		if (GetLastError() == 5)
		{
			printf("[WINRECOPY] File is not found\n");
			FindClose(FindDataHandle);
		}
		else
		{
			if (FindDataHandle == ERROR_ACCESS_DENIED)
			{
				printf("[WINRECOPY] Try and run as an admin\n");

			}
			exit(EXIT_FAILURE);
		}
	}
	do
	{
		if (GetDirData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(GetDirData.cFileName, "..") == 0 || strcmp(GetDirData.cFileName, ".") == 0)
			{
				continue;
			}
			else
			{
				

				sprintf_s(SrcBuff, sizeof(SrcBuff), "%s\\%s", DirFrom, GetDirData.cFileName);
				sprintf_s(DstBuff, sizeof(DstBuff), "%s\\%s", DirToClean, GetDirData.cFileName);

				BOOL CopyDir = CreateDirRecur(DstBuff);
				if (!CopyDir)
				{
					if (GetLastError() == 183)
					{
						printf("[WINRECOPY] Dir: %s already exists at %s\n", DstBuff, SrcBuff);
					}
					else
					{
						printf("[WINRECOPY] Failed to get create directory %s. ERROR CODE: %d\n", DstBuff, GetLastError());
						FindClose(FindDataHandle);
						exit(EXIT_FAILURE);
					}
				}
				CopyDirRecursive(SrcBuff, DstBuff);
			}
		}
		else
		{
			
			sprintf_s(SrcBuff, sizeof(SrcBuff), "%s\\%s", DirFrom, GetDirData.cFileName);
			sprintf_s(DstBuff, sizeof(DstBuff), "%s\\%s", DirTo, GetDirData.cFileName);

			if (strpbrk(GetDirData.cFileName, illegalChars) != NULL)
			{
				printf("Skipping %s due to invalid chars\n", SrcBuff);
			}
			else
			{
				BOOL CopyF = CopyFileA(SrcBuff, DstBuff, TRUE);
				if (!CopyF)
				{
					if (GetLastError() == ERROR_SHARING_VIOLATION)
					{
						printf("[WINRECOPY] %s is in use by another process\n", SrcBuff);
					}
					else if (GetLastError() == ERROR_ALREADY_EXISTS)
					{
						printf("[WINRECOPY %s already exists in %s\n", SrcBuff, DstBuff);
					}
					else
					{
						printf("[WINRECOPY] Failed to copy file %s to %s. ERROR CODE: %d\n", SrcBuff, DstBuff, GetLastError());
						FindClose(FindDataHandle);
						exit(EXIT_FAILURE);
					}
				}
				else
				{
					printf("Copied bytes from %s -> %s\n", SrcBuff, DstBuff);
				}
			}
		}
	} while (FindNextFileA(FindDataHandle, &GetDirData));

	if (GetLastError() != ERROR_NO_MORE_FILES)
	{
		printf("[WINRECOPY] Cannot enumerate directory. ERROR CODE: %d\n", GetLastError());
		FindClose(FindDataHandle);
		exit(EXIT_FAILURE);
	}

	FindClose(FindDataHandle);
}

#endif