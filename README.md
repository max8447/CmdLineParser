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

See Example.cpp for a ready-to-use solution.

Any contributions are welcome.
