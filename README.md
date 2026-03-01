# CmdLineParser
Lightweight commandline parser single-header library in C++

# TODO
- Add option for default value

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
`Type`: Type of the argument<br>
`Name`: Name of the argument (the name is arbitrary)<br>
`CmdLineArg`: The flag for which is searched<br>
`bNextValue`: Whether or not to use the next argument for the value<br>

2. Call the `DEFINE_CMDLINE` macro:
```cpp
DEFINE_CMDLINE(CommandLine);
```

Parameters:<br>
`CmdLineName`: Name of the command line object

3. Call the `PARSE_CMDLINE` macro:
```cpp
bool bSuccess = PARSE_CMDLINE(CommandLine, argc, argv);
```

Parameters:<br>
`CmdLine`: Command line object<br>
`argc`: Number of arguments (passed to `main` by argc)<br>
`argv`: Pointer to arguments (passed to `main` by argv)<br>

Return value:<br>
`true` on success, `false` on failure (failures only happen when a commandline arg is flagged to use the next arg as the value but there is no next arg, or if we aren't on little-endian).<br>

See [Example.cpp](https://github.com/max8447/CmdLineParser/blob/main/Example.cpp) for a ready-to-use solution.

Any contributions are welcome.
