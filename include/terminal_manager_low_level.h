#ifndef __BU_TERMINAL_MANAGER_LOW__
#define __BU_TERMINAL_MANAGER_LOW__

/** \brief A function that prints out a character stored in a \link tm_colored_char tm_colored_char\endlink with its set colors and transparency.
*
* \param colored_character The colored character to be printed.
*/
void tm_printChar(tm_colored_char colored_character);

/** \brief A function that prints out a unicode character stored in a \link tm_uchar tm_uchar\endlink with its set colors and transparency.
*
* \param c The colored unicode character to be printed.
*/
void tm_print_uchar(tm_uchar c);

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

void tm_resetColor();

/** \brief Write to the console the requiered string to switch text color to the given color.
* \param color The color you want as text color
*/
void tm_set_fg(tm_color color);
/** \ingroup CFs
* \brief Write to the console the requiered string to switch background color to the given color
 * \param color The color you want as background color
*/
void tm_set_bg(tm_color color);
void tm_moveCursor(int row, int column);

#endif
