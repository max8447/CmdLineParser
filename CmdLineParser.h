/*
	CmdLineParser.h - lightweight commandline parser single-header library in C++

	Copyright (c) 2026 Max Müller
	https://github.com/max8447/CmdLineParser

	MIT License
*/

#pragma once

#include <cstddef>
#include <cstring>
#include <type_traits>
#include <cmath>
#include <cctype>

#ifdef __cpp_if_constexpr
#define CLP_IF_CONSTEXPR constexpr
#else
#define CLP_IF_CONSTEXPR
#endif

#define DEFINE_CMDLINE_ARG(Type, Name, CmdLineArg, bNextValue)		\
Type Name;

#define DEFINE_TRANSLATOR_ARG(Type, Name, CmdLineArg, bNextValue)	\
Arg Name = Arg(														\
CmdLineArg,															\
bNextValue,															\
offsetof(CmdLine_struct, Name),										\
sizeof(Type),														\
Parse<Type>															\
);

#define DEFINE_CMDLINE(CmdLineName)									\
struct CmdLine_struct												\
{																	\
DEFINE_CMDLINE_FIELDS(DEFINE_CMDLINE_ARG);							\
}CmdLineName{};														\
struct																\
{																	\
DEFINE_CMDLINE_FIELDS(DEFINE_TRANSLATOR_ARG);						\
}Translator;

#define PARSE_CMDLINE(CmdLine, argc, argv)							\
(ParseCmdLine(argc, argv, &(CmdLine), sizeof(CmdLine), &(Translator), sizeof(Translator)))

// you never know
bool IsUsingLittleEndian()
{
	unsigned short Test = 0x0001;

	return *(unsigned char*)&Test == 0x01;
}

// case insensitive
bool StartsWith(char* String, const char* Pattern)
{
	size_t PatternLen = strlen(Pattern);

	if (strlen(String) < PatternLen)
	{
		return false;
	}

	for (int i = 0; i < PatternLen; i++)
	{
		if (tolower(String[i]) != tolower(Pattern[i]))
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

	if CLP_IF_CONSTEXPR (std::is_integral<Type>::value && !std::is_same<typename std::remove_cv<Type>::type, bool>::value)
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
		
		if CLP_IF_CONSTEXPR (std::is_unsigned<Type>::value)
		{
			ReturnValue = new unsigned long long(strtoull(CmdLineArg + ToSkip, nullptr, Base));
		}
		else if CLP_IF_CONSTEXPR (std::is_signed<Type>::value)
		{
			ReturnValue = new long long(strtoll(CmdLineArg + ToSkip, nullptr, Base));
		}
	}
	else if CLP_IF_CONSTEXPR (std::is_same<Type, bool>::value)
	{
		ReturnValue = new bool(!!strtol(CmdLineArg, nullptr, 10) /* || strstr?? */);

		if (strcmp(CmdLineArg, OptionalArg) == 0) // for when only a single arg sets it to true (e.g -d)
		{
			*(bool*)ReturnValue = true;
		}
	}
	else if CLP_IF_CONSTEXPR (std::is_same<Type, float>::value)
	{
		ReturnValue = new float(strtof(CmdLineArg, nullptr));
	}
	else if CLP_IF_CONSTEXPR (std::is_same<Type, double>::value)
	{
		ReturnValue = new double(strtod(CmdLineArg, nullptr));
	}
	else if CLP_IF_CONSTEXPR (std::is_same<Type, long double>::value)
	{
		ReturnValue = new long double(strtold(CmdLineArg, nullptr));
	}
	else if CLP_IF_CONSTEXPR (std::is_pointer<Type>::value)
	{
		if CLP_IF_CONSTEXPR (std::is_same<typename std::remove_const<typename std::remove_pointer<Type>::type>::type, char>::value)
		{
			ReturnValue = new char* (_strdup(CmdLineArg));
		}
		else if CLP_IF_CONSTEXPR (std::is_same<typename std::remove_const<typename std::remove_pointer<Type>::type>::type, wchar_t>::value)
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

	inline Arg(const char* InCmdLine, bool InbUseNextValue, int InOffset, int InSize, void* (*InParse)(char* CmdLineArg, char* OptionalArg))
		: CmdLine(InCmdLine)
		, bUseNextValue(InbUseNextValue)
		, Offset(InOffset)
		, Size(InSize)
		, Parse(InParse)
	{
	}
};

bool ParseCmdLine(int argc, char** argv, void* CmdLine, size_t SizeOfCmdLine, const void* Translator, size_t SizeOfTranslator)
{
	if (!IsUsingLittleEndian())
	{
		// only support little endian because we don't copy all bytes of integrals
		return false;
	}

	if (!CmdLine || !Translator)
	{
		return false;
	}

	memset(CmdLine, 0, SizeOfCmdLine);

	if (argc <= 1) // there should always be one, but just in case the user emits the first arg
	{
		return true; // return true because this technically isn't a fail case (program should handle case where user doesn't use any args)
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
