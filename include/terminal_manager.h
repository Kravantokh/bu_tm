/* Tamás Benő 4/22/2022 */
#ifndef __BU_TERMINAL_MANAGER__
#define __BU_TERMINAL_MANAGER__

#ifndef USE_IN_MEMORY_CHARACTER_MAPS
	#define USE_IN_MEMORY_CHARACTER_MAPS 1
#endif

#ifndef PREDEFINE_CHARACTERS
	#define PREDEFINE_CHARACTERS 1
#endif

#ifndef PREDEFINE_COLORNAMES
	#define PREDEFINE_COLORNAMES 1
#endif

#ifndef HANDLE_MAIN
	#define HANDLE_MAIN 1
#endif

#include "tm_predefs.h"
#include "tm_main.h"

/* Global variables */
#if USE_IN_MEMORY_CHARACTER_MAPS
	#ifdef __cplusplus
	namespace bu{
	namespace tm{
	#endif
		extern unsigned char hatch_map[3];
		extern unsigned char table_map[22];
	#ifdef __cplusplus
	}
	}
	#endif
#endif

extern int tm_run;

/*Make it easier for c++ users to use this header.*/
#ifdef __cplusplus
/*benutils namespace*/
namespace bu{

/*terminal manager namespace*/
namespace tm{


extern "C" {
#endif

/****  Functions  ****/

/*Initialize terminal manager. */
void initTerminalManager();

/*Gives you the size of the terminal in the two integers you pass it.*/
void getTerminalSize(int* rows, int* columns);

/* Move the cursor to a given position*/
void moveTerminalCursorTo(int row, int column);

/*Clear the terminal by running cls on windows and clear on linux.
Note: This should not be used for frequent refreshes because it may take time and make everyting flash.*/
void clearTerminal();

/*Wait for the specified amount of seconds.*/
void wait(unsigned int t);
/*Wait for the specified amount of microseconds.*/
void waitus(unsigned int t);

/* Set rgb color for true color terminals */

/* Get text color string into the given string. It should be at least 22 characters long. */
void store_rgbFG(char* s, int red, int green, int blue);
/* Get background color string into the given string. It should be at least 22 characters long. */
void store_rgbBG(char* s, int red, int green, int blue);
/* Reset text and background color to default */
void resetColor();

/* Write to the console the requiered string to switch text color to the given RGB color */
void rgbFG(int red, int green, int blue);
/* Write to the console the requiered string to switch background color to the given RGB color */
void rgbBG(int red, int green, int blue);

/* Get text color string into the given string. It should be at least 22 characters long. */
void store_hexFG(char* s, const char* val);
/* Get background color string into the given string. It should be at least 22 characters long. */
void store_hexBG(char* s, const char* val);
/* Write to the console the requiered string to switch text color to the given RGB color */
void hexFG(const char* val);
/* Write to the console the requiered string to switch background color to the given RGB color */
void hexBG(const char* val);

#if HANDLE_MAIN

extern void setCharCallback(const char character, void(*function)(void));
extern void callCharCallback(char character);

#endif

#ifdef __cplusplus
//Here I can finally use c++ comments, cuz it will not be visible for the c compiler and thus compatibility all the way back to c89 will be kept.

} //end of extern C functions.

} //namespace tm
} //namespace bu
#endif

#endif
