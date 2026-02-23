#include "CmdLineParser.h"

#include <cstdio>

int main(int argc, char** argv)
{
#define DEFINE_CMDLINE_FIELDS(Define)			\
Define(int, arg1, "-d", true);					\
Define(bool, arg2, "-e", false);				\
Define(char*, arg3, "-p", true);				\
Define(float, arg4, "-f", true);

	DEFINE_CMDLINE(CmdLine);

	if (!PARSE_CMDLINE(CmdLine, argc, argv))
	{
		printf("Failed to parse command line!\n");
		return 1;
	}

	printf("CmdLine:\n");
	printf("arg1: %d\n", CmdLine.arg1);
	printf("arg2: %s\n", CmdLine.arg2 ? "true" : "false");
	printf("arg3: %s\n", CmdLine.arg3);
	printf("arg4: %f\n", CmdLine.arg4);

	return 0;
}