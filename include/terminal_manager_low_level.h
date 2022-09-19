/** \file terminal_manager_low_level.h
* \brief A header if you don't want to use all the high level features provided by the library.
*/
#ifndef __BU_TERMINAL_MANAGER_LOW__
#define __BU_TERMINAL_MANAGER_LOW__

#include "terminal_manager.h"

/** A simple initialization function for the library.
* Should always be run if you implement the main function.
*/
void tm_init();

/** \brief A function that prints out a character stored in a \link tm_colored_char tm_colored_char\endlink with its set colors and transparency.
*
* \param colored_character The colored character to be printed.
*/
void tm_print_colored_char(tm_colored_char colored_character);

/** \brief A function that prints out a unicode character stored in a \link tm_colored_uchar tm_colored_uchar\endlink with its set colors and transparency.
*
* \param c The unicode character to be printed.
*/
void tm_print_colored_uchar(tm_colored_uchar c);

/** \brief Write to the console the requiered string to switch text color to the given RGB value
* \param red Red value (0-255)
* \param green Green value (0-255)
* \param blue Blue value (0-255)
*/
void tm_set_fg(tm_color color);
/** \ingroup CFs
* \brief Write to the console the requiered string to switch background color to the given RGB value
 * \param color
*/
void tm_set_bg(tm_color color);

/** \ingroup CFs
* \brief Write to the console the requiered escape sequence to switch the text color to the given HEX value
*
* \param value Provide a string with a length of at least 22 characters to store the escape sequence.
* \param value The hex value you want to use. You may choose to use a starting \a # or omit it if you wish.
*/

void tm_reset_color();

/** \brief Write to the console the requiered string to switch text color to the given color.
* \param color The color you want as text color
*/
void tm_set_fg(tm_color color);
/** \ingroup CFs
* \brief Write to the console the requiered string to switch background color to the given color
 * \param color The color you want as background color
*/
void tm_set_bg(tm_color color);

void tm_move_cursor(int row, int column);

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

#endif
