#include <Windows.h>
#include <stdio.h>

#include "libwinrecopy.h"


void Help()
{
	printf("[WINRECOPY] Help\n"
		"Author: Adam Croft(Adock90)\n"
		"Date Created: 25/12/25\n"
		"Documentation\n"
		"\twinrecopy.exe [<SourcePath>] [<DestinationPath>]\n"
		"\t\t[<SourcePath>]: The Starting Point of where you want your files to be copied from\n"
		"\t\t[<DestinationPath>]: The Ending Point of where you want your file to be copied to\n"
		"Examples:\n"
		"\t\twinrecopy.exe C:\ProgramFiles F:\ \n"
		"\t\twinrecopy.exe C:\Users\User\Documents C:\Documents\n"
	);
}

int main(int argc, char* argv[])
{
	//if (argc < 2 || argc > 2)
	//{
		//Help();
		//return 1;
	//}

	CopyDirRecursive(argv[1], argv[2]);

	return 0;
}