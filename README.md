# Benoe's utilities: Terminal Manager #
## Descripiton
A C/C++ cross-platform library made to ease the development of console applications.

 Currently supported platforms are: Linux, Windows 10 (11 untested, but there is no reason for it not to work). The library has functions to manage terminal size detection, resizing, text and background color changing and provides utilities for unicode text manipulation (with the help of my of my other library added as a submodule).

Consider generating and reading the docs!

## Prerequisites
 * GCC
 * Doxygen (if you want to generate the docs)

## Compilation
Make sure to clone it with ``git clone --recursive https://github.com/Kravantokh/benutils_terminal_manager`` or else you may run into problems due to missing submodules!
If you have already cloned it not recursively run:
```
git submodule init
git submodule update
```

Run `compile.sh` or `compile.bat` depending on your platform to compile the program.

## Documentation

To create the documentation run `makedocs.sh` or `makedocs.bat` according to your platform.

## Planned Features
 * Support for CMake.
 *  Pre-defined window elements with automatic resizing such as textboxes, drawboxes, lists etc.
