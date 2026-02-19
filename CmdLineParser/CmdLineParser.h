/*
	CmdLineParser.h - lightweight commandline parser single-header library in C++

	Copyright (c) 2026 Max Müller
	https://github.com/max8447/CmdLineParser

	MIT License
*/

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

wchar_t* NCStrToWCStr(char* NCStr)
{
	size_t WCStrLen;
	mbstowcs_s(&WCStrLen, NULL, 0, NCStr, 0);

	wchar_t* WCStr = new wchar_t[WCStrLen + 1];

	mbstowcs_s(&WCStrLen, WCStr, WCStrLen, NCStr, WCStrLen + 1);
	
	return WCStr;
}

template<typename Type>
void* Parse(char* CmdLineArg, char* OptionalArg)
{
	void* ReturnValue = nullptr;

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
			ReturnValue = new unsigned long long(strtoull(CmdLineArg + ToSkip, nullptr, Base));
		}
		else if constexpr (std::is_signed_v<Type>)
		{
			ReturnValue = new long long(strtoll(CmdLineArg + ToSkip, nullptr, Base));
		}
	}
	else if constexpr (std::is_same_v<Type, bool>)
	{
		ReturnValue = new bool(!!atoi(CmdLineArg) /* || strstr?? */);

		if (strcmp(CmdLineArg, OptionalArg) == 0) // for when only a single arg sets it to true (e.g -d)
		{
			*(bool*)ReturnValue = true;
		}
	}
	else if constexpr (std::is_same_v<Type, float>)
	{
		ReturnValue = new float(atof(CmdLineArg));
	}
	else if constexpr (std::is_same_v<Type, double>)
	{
		ReturnValue = new double(atof(CmdLineArg));
	}
	else if constexpr (std::is_pointer_v<Type>)
	{
		if constexpr (std::is_same_v<std::remove_const_t<std::remove_pointer_t<Type>>, char>)
		{
			ReturnValue = new char* (_strdup(CmdLineArg));
		}
		else if constexpr (std::is_same_v<std::remove_const_t<std::remove_pointer_t<Type>>, wchar_t>)
		{
			ReturnValue = new wchar_t* (NCStrToWCStr(CmdLineArg));
		}
	}

	return ReturnValue;
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

	for (int i = 0; i < argc; i++)
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

				if (ParsedArg)
				{
					memcpy((void*)(size_t(CmdLine) + TranslatorArg->Offset), ParsedArg, TranslatorArg->Size);
				}
			}
		}
	}

	return true;
}
