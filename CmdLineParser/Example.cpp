#include "CmdLineParser.h"

#include <cstdio>

int main(int argc, char** argv)
{
#define DEFINE_CMDLINE_FIELDS(Define)			\
Define(int, arg1, "-d", true);					\
Define(bool, arg2, "-e", false);				\
Define(char*, arg3, "-p", true);				\
Define(float, arg4, "-f", true);

	DEFINE_CMDLINE();

	CmdLine CmdLine;
	if (!ParseCmdLine(argc, argv, &CmdLine, sizeof(CmdLine), &Translator, sizeof(Translator)))
	{
		printf("Failed to parse command line!\n");
		return 1;
	}

	// use CmdLine here

	return 0;
}