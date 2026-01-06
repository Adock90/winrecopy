#ifndef LIBWINRECOPY_H
#define LIBWINRECOPY_H

#include <Windows.h>
#include <stdio.h>

void CheckDirExists(char* Path, BOOL Correct);

void CopyDirRecursive(char* DirIter, char* DirTo);

#endif