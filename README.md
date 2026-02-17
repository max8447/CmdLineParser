# CmdLineParser
Lightweight commandline parser single-header library in C++

# How to use

1. Define `DEFINE_CMDLINE_FIELDS` macro with the corresponding fields for your command line arguments:
```cpp
#define DEFINE_CMDLINE_FIELDS(Define)           \
Define(int, arg1, "-d", true);					\
Define(bool, arg2, "-e", false);				\
Define(char*, arg3, "-p", true);				\
Define(float, arg4, "-f", true);
```

Parameters:<br>
Type of the argument<br>
Name of the argument (the name is arbitrary)<br>
The flag for which is searched<br>
Whether or not to use the next argument for the value<br>

2. Call the `DEFINE_CMDLINE` macro:
```cpp
DEFINE_CMDLINE();
```

3. Define command line variable of type CmdLine:
```cpp
CmdLine CommandLine;
```

4. Call ParseCmdLine function:
```cpp
bool bSuccess = ParseCmdLine(argc, argv, &CommandLine, sizeof(CmdLine), &Translator, sizeof(Translator);
```

Parameters:<br>
Number of arguments (passed to main by argc)<br>
Pointer to arguments (passed to main by argv)<br>
Pointer to CmdLine variable we just created<br>
Size of CmdLine<br>
Pointer to Translator (variable defined by the DEFINE_CMDLINE macro)<br>
Size of Translator<br>

Return value:<br>
true on success, false on failure (failures only happen when a commandline arg is flagged to use the next arg as the value but there is no next arg).<br>

See Example.cpp for a ready-to-use solution.

Any contributions are welcome.
