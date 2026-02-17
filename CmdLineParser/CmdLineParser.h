#pragma once

#include <stddef.h>
#include <cstring>
#include <type_traits>
#include <cmath>

#define DEFINE_CMDLINE_ARG(Type, Name, CmdLineArg, bNextValue) \
Type Name;

#define DEFINE_TRANSLATOR_ARG(Type, Name, CmdLineArg, bNextValue)												\
Arg Name = Arg{																									\
.CmdLine = CmdLineArg,																							\
.bUseNextValue = bNextValue,																					\
.Offset = offsetof(CmdLine, Name),																				\
.Size = sizeof(Type),																							\
.Parse = Parse<Type>\
};

#define DEFINE_CMDLINE()													\
struct CmdLine																\
{																			\
DEFINE_CMDLINE_FIELDS(DEFINE_CMDLINE_ARG);									\
};																			\
constexpr struct															\
{																			\
DEFINE_CMDLINE_FIELDS(DEFINE_TRANSLATOR_ARG);								\
}Translator{};

bool StartsWith(char* String, const char* Pattern)
{
	size_t PatternLen = strlen(Pattern);

	if (strlen(String) < PatternLen)
	{
		return false;
	}

	for (int i = 0; i < PatternLen; i++)
	{
		if (String[i] != Pattern[i])
		{
			return false;
		}
	}

	return true;
}

template<typename Type>
void* Parse(char* CmdLineArg, char* OptionalArg)
{
	if constexpr (std::_Is_nonbool_integral<Type>)
	{
		int Base = 10;
		int ToSkip = 0;

		if (StartsWith(CmdLineArg, "0x"))
		{
			Base = 16;
			ToSkip = 2;
		}
		else if (StartsWith(CmdLineArg, "0b"))
		{
			Base = 2;
			ToSkip = 2;
		}
		else if (StartsWith(CmdLineArg, "0"))
		{
			Base = 8;
			ToSkip = 1;
		}

		if constexpr (std::is_unsigned_v<Type>)
		{
			unsigned long long* ReturnValue = new unsigned long long(strtoull(CmdLineArg + ToSkip, nullptr, Base));

			return ReturnValue;
		}
		else if constexpr (std::is_signed_v<Type>)
		{
			long long* ReturnValue = new long long(strtoll(CmdLineArg + ToSkip, nullptr, Base));

			return ReturnValue;
		}
	}
	else if constexpr (std::is_same_v<Type, bool>)
	{
		bool* ReturnValue = new bool(!!atoi(CmdLineArg) /* || strstr?? */);

		if (strcmp(CmdLineArg, OptionalArg) == 0) // for when only a single arg sets it to true (e.g -d)
		{
			*ReturnValue = true;
		}

		return ReturnValue;
	}
	else if constexpr (std::is_same_v<Type, float>)
	{
		float* ReturnValue = new float(atof(CmdLineArg));

		return ReturnValue;
	}
	else if constexpr (std::is_same_v<Type, double>)
	{
		double* ReturnValue = new double(atof(CmdLineArg));

		return ReturnValue;
	}
	else if constexpr (std::is_pointer_v<Type> && std::is_same_v<std::remove_const_t<std::remove_pointer_t<Type>>, char>)
	{
		char** ReturnValue = new char*(_strdup(CmdLineArg));

		return ReturnValue;
	}

	return 0;
}

struct Arg
{
	const char* CmdLine;
	bool bUseNextValue;
	int Offset;
	int Size;
	void* (*Parse)(char* CmdLineArg, char* OptionalArg);
};

bool ParseCmdLine(int argc, char** argv, void* CmdLine, size_t SizeOfCmdLine, const void* Translator, size_t SizeOfTranslator)
{
	if (!CmdLine || !Translator)
	{
		return false;
	}

	memset(CmdLine, 0, SizeOfCmdLine);

	if (argc <= 1) // there should always be one, but just in case the user emits the first arg
	{
		return false;
	}

	for (int i = 1; i < argc; i++)
	{
		char* arg = argv[i];

		for (const Arg* TranslatorArg = (Arg*)Translator; TranslatorArg != (Arg*)(size_t(Translator) + SizeOfTranslator); TranslatorArg++)
		{
			if (strcmp(arg, TranslatorArg->CmdLine) == 0)
			{
				char* CmdLineArg = arg;

				if (TranslatorArg->bUseNextValue)
				{
					if (i + 1 >= argc)
					{
						return false;
					}

					i++;
					CmdLineArg = argv[i];
				}

				void* ParsedArg = (void*)TranslatorArg->Parse(CmdLineArg, arg);

				memcpy((void*)(size_t(CmdLine) + TranslatorArg->Offset), ParsedArg, TranslatorArg->Size);
			}
		}
	}

	return true;
}