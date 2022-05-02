/** \file terminal_manager.h
* \brief The main header file of the library.
*/
#ifndef __BU_TERMINAL_MANAGER__
#define __BU_TERMINAL_MANAGER__

/** \defgroup Settings
* Library settings. Their default values ar shown by Doxygen as their value. You can override these if you define them before including the main header file.
*/

/** \addtogroup Settings
 * @{
 */
/**
* \brief This macro decides whether some predefined character maps should be loaded into memory or not.
*
* For more information about these character maps see \link MMC Memory Mapped Characters\endlink.
*/
#ifndef TM_USE_IN_MEMORY_CHARACTER_MAPS
	#define TM_USE_IN_MEMORY_CHARACTER_MAPS 1
#endif

/**
* \brief This macro decides whether some macros for characters should be created or not.
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

/* Global variables */
#if TM_USE_IN_MEMORY_CHARACTER_MAPS
	#ifdef __cplusplus
	namespace bu{
	namespace tm{
	#endif
	/** \defgroup MMC Memory Mapped Characters*/

		/** \ingroup MMC
		* \brief A character map that contatins hatch characters.
		* It's contents are: {░, ▒, ▓}
		*/
		extern unsigned char tm_hatch_map[3];
		/** \ingroup MMC
		* \brief A character map that contatins characters for drawing tables.
		* It's contents are: {┌, ┐, └, ┘, │, ─, ├, ┤, ┴, ┬, ┼, ╔, ╗, ╚, ╝, ║, ═, ╠, ╣, ╩, ╦, ╬}
		*/
		extern unsigned char tm_table_map[22];
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

/** \brief Move the cursor to a given position. The position is given in character indices.
*/
void tm_moveCursor(int row, int column);

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

/** \ingroup CFs
* \brief Store the text color changing escape sequence corresponding to the given RGB value into the given string.
*
* \param s Provide a string with a length of at least 22 characters to store the escape sequence.
* \param red Red value (0-255)
* \param green Green value (0-255)
* \param blue Blue value (0-255)
*/
void tm_store_rgbFG(char* s, int red, int green, int blue);
/** \ingroup CFs
* \brief Store the background color changing escape sequence corresponding to the given RGB value into the given string.
*
* \param s Provide a string with a length of at least 22 characters to store the escape sequence.
* \param red Red value (0-255)
* \param green Green value (0-255)
* \param blue Blue value (0-255)
*/
void tm_store_rgbBG(char* s, int red, int green, int blue);

/** \ingroup CFs
* \brief Print an escape sequence that resets the colors to the default.
*/
void tm_resetColor();

/** \brief Write to the console the requiered string to switch text color to the given RGB value
* \param red Red value (0-255)
* \param green Green value (0-255)
* \param blue Blue value (0-255)
*/
void tm_rgbFG(int red, int green, int blue);
/** \ingroup CFs
* \brief Write to the console the requiered string to switch background color to the given RGB value
* \param red Red value (0-255)
* \param green Green value (0-255)
* \param blue Blue value (0-255)
*/
void tm_rgbBG(int red, int green, int blue);

/** \ingroup CFs
* \brief Store the text color changing escape sequence corresponding to the given HEX value into the given string.
*
* \param s Provide a string with a length of at least 22 characters to store the escape sequence.
* \param value The hex value you want to use. You may choose to use a starting \a # or omit it if you wish.
*/void tm_store_hexFG(char* s, const char* value);

/** \ingroup CFs
* \brief Store the background color changing escape sequence corresponding to the given HEX value into the given string.
* You may choose to use a starting \a # or omit it if you wish.
* \param s Provide a string with a length of at least 22 characters to store the escape sequence.
* \param value The hex value you want to use. You may choose to use a starting \a # or omit it if you wish.
*/
void tm_store_hexBG(char* s, const char* value);

/** \ingroup CFs
* \brief Write to the console the requiered escape sequence to switch the text color to the given HEX value
*
* \param value Provide a string with a length of at least 22 characters to store the escape sequence.
* \param value The hex value you want to use. You may choose to use a starting \a # or omit it if you wish.
*/
void tm_hexFG(const char* value);
/** \ingroup CFs
* \brief Write to the console the requiered escape sequence to switch the backgorund color to the given HEX value.
*
* \param value Provide a string with a length of at least 22 characters to store the escape sequence.
* \param value The hex value you want to use. You may choose to use a starting \a # or omit it if you wish.
*/
void tm_hexBG(const char* value);

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
void setResizeCallback(void(*function)(int, int));

#endif

#ifdef __cplusplus
//Here I can finally use c++ comments, cuz it will not be visible for the c compiler and thus compatibility all the way back to c89 will be kept.

} //end of extern C functions.

} //namespace tm
} //namespace bu
#endif

#endif

/** \mainpage Terminal Manager
* A C/C++ cross-platform library made to ease the development of console applications.
* Currently supported platforms are: Linux, Windows 10 (11 has not been tested but should work). The library has functions to manage terminal size detection, resizing, text and background color changing and provides many macros and definitions to help with the use of the extended ASCII character set.
*
* Given how simplistic this library is you should check out the example codes given and be able to figure out most of how it works from there. Other than that you should check out the \link terminal_manager.h this\endlink tab above to see the documentation of most of the functions.
*/
