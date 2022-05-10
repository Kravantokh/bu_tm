/** \file terminal_manager.h
* \brief The main header file of the library.
*/
#ifndef __BU_TERMINAL_MANAGER__
#define __BU_TERMINAL_MANAGER__

/** \defgroup Settings
* Library settings. Their default values ar shown by Doxygen as their value. You can override these if you define them before including the main header file.
*/

/**
* \brief This macro decides whether macros for characters should be created or not.
*
* It makes use of \link tm_predefs.h tm_predefs.h\endlink.
*/
#ifndef TM_PREDEFINE_CHARACTERS
	#define TM_PREDEFINE_CHARACTERS 1
#endif

/**
* \brief This macro decides whether some macros for terminal colors should be created or not.
*
* It makes use of \link tm_predefs.h tm_predefs.h\endlink.
*/
#ifndef TM_PREDEFINE_COLORNAMES
	#define TM_PREDEFINE_COLORNAMES 1
#endif

/**
* \brief This macro decides whether the library should handle the main. If set to 0 you have to implement the main function on your own.
* \see tm_initCall
*/
#ifndef TM_HANDLE_MAIN
	#define TM_HANDLE_MAIN 1
#endif

/** @}*/
#include "tm_predefs.h"
#include <stdint.h>
#include <wchar.h>

/* Global variables */

extern int tm_run;

/*  Structs  */
/**\brief A struct to store rgb colors and alpha.
*
* The alpha channel is used as a boolean. It may be useful on transparent terminals or terminal which have a background color other than black. It may be used to print the reset color escape sequence instead of setting the background to black.
*/
struct tm_color{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};
typedef struct tm_color tm_color;

/** \brief A struct intented to store a foreground (text) and background color along with a character.
*
* Use this in conjuction with \link tm_printChar tm_printChar\endlink to print out colored characters.
*/
struct tm_colored_char{
	struct tm_color fg;
	struct tm_color bg;
	char ch;
};
typedef struct tm_colored_char tm_colored_char;


/** \brief A struct intented to store a foreground (text) and background color along with a unicode character.
*
* Use this in conjuction with \link tm_print_uchar tm_print_uchar\endlink to print out colored characters.
*/
struct tm_colored_uchar{
	struct tm_color fg;
	struct tm_color bg;
	char* ch;
};
typedef struct tm_colored_uchar tm_colored_uchar;


#ifdef __cplusplus
/*benutils namespace*/
namespace benutils{

/*terminal manager namespace - removed due to being redundant because of the tm_ prefixes*/
/*namespace tm{
*/

extern "C" {
#endif

/****  Functions  ****/

/*  Struct Managers  */

/** \brief A function that prints out a unicode character stored in a \link tm_colored_uchar tm_colored_uchar\endlink with its set colors and transparency.
*
* \param c The unicode character to be printed.
*/
void tm_print_colored_uchar(tm_colored_uchar c);

/*  Struct manager functions  */

/** \brief A function that returns a \link tm_color tm_color\endlink struct containing the given color.
*
* \param r Red value (0-255)
* \param g Green value (0-255)
* \param b Blue value (0-255)
* \param a Alpha (0-1)
*/
tm_colored_char tm_create_colored_char(tm_color fg, tm_color bg, char c);


/** A simple initialization function for the library.
* Should always be run if you implement the main function.
*/
void tm_init();

#if TM_HANDLE_MAIN
/**\defgroup HM Functions for managing the terminal. TM_HANDLE_MAIN must be defined for these to work.*/

/** \ingroup HM
* \brief This function is called when the application starts.
*
* Define it with your initialization code inside it (keybindigs should also be added here). Think of it as the code that runs in the main before the main loop. In the loop all you have are the callbacks to perform actions.
* \n \b Attention! If you do not define this you will have linking errors unless you set \a TM_HANDLE_MAIN to 0.
*/
void tm_initCall();
#endif

/**
* \brief Gives you the size of the terminal.
* \param[out] rows The vertical size of the terminal will be written to this address in number of characters.
* \param[out] columns The horizontal size of the terminal will be written to this address in number of characters.
*/
void tm_getTerminalSize(int* rows, int* columns);

/** \brief Clears the terminal

* \b Note: This should not be used for frequent refreshes because it may take time and make everyting flash.
*/
void tm_clear();

/** \brief Wait for the specified amount of seconds.*/
void tm_wait(unsigned int t);
/** \brief Wait for the specified amount of microseconds.
*
* Not absolutely precise. May wait a little more.*/
void tm_waitus(unsigned int t);

/* Set rgb color for true color terminals */

/** \defgroup CFs Color changing functions*/

tm_color tm_create_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/** \ingroup CFs
* \brief A function that returns a \link tm_color tm_color\endlink struct containing the given color.
*
* \param s A string containing the hex color. ou may choose to use a starting \a # or omit it if you wish.
* \param a Alpha (0-1)
*/

tm_color tm_create_hex_color(char* s, uint8_t a);


#if TM_HANDLE_MAIN
/** \ingroup HM
* Bind a function callback to a key.
*\brief Whenever the key is pressed the given function will be called
*
*\param character The ASCII value (or character) to which the given function should be bound.
\param function A pointer to the function that you wish to bind to the key. It should be of type void and take no arguments.
*/
void tm_bindKey(const char character, void(*function)(void));
/** \ingroup HM
* \brief Bind a function callback for every key.
*
* Whenever any key is pressed the given function will be called. This works alongside \link tm_bindKey tm_bindKey\endlink and does not influence those bindings.
\param function A pointer to the function that you wish to bind. It should be of type void and take no arguments.
*/
void tm_bindToAnyKeypress(void(*function)(char));
/** \ingroup HM
* \brief Bind a function callback for whenever the terminal is resized.
*
* \param function The function to be called whenever the terminal is resized. It should be of type \a void and take two \a int arguments (new number of rows and columns respectively)/
*/
void tm_setResizeCallback(void(*function)(int, int));

#endif

#ifdef __cplusplus
//Here I can finally use c++ comments, cuz it will not be visible for the c compiler and thus compatibility all the way back to c89 will be kept.

} //end of extern C functions.

//} //namespace tm
} //namespace bu
#endif

#endif

/** \mainpage Terminal Manager
* A C/C++ cross-platform library made to ease the development of console applications.
* Currently supported platforms are: Linux, Windows 10 (11 has not been tested but should work). The library has functions to manage terminal size detection, resizing, text and background color changing and provides many macros and definitions to help with the use of the extended ASCII character set.
* On windows the terminal font must be changed by the program to guarantee that unicode characters can be displayed.
*
* Given how simplistic this library is you should check out the example codes given and be able to figure out most of how it works from there. Other than that you should check out the \link terminal_manager.h this\endlink tab above to see the documentation of most of the functions.
*
*
*/
