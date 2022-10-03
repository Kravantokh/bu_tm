#ifndef __BU_TERMINAL_MANAGER_ELEMENTS__
#define __BU_TERMINAL_MANAGER_ELEMENTS__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* A global error function which you can replace with a custom function */
#ifndef BU_ERR_FUNC
#define BU_ERR_FUNC(E) fprintf(stderr, E);
#endif

/* Should the library handle the main loop and input processing or are you willing to implement them for yourself? */
#ifndef TM_HANDLE_MAIN
	#define TM_HANDLE_MAIN 1
#endif


/************* Struct forward declarations and globals ************/
/* This section is meant to remove the clutter caused by all the structs upfront.
 * Most of these structs are used in the mid and low level APIs anyways, but
 * a short description of their roles will pe provided here. */

typedef struct _tm_composable tm_composable;

/* Vertical and horizontal divisions */
typedef struct _tm_vdiv tm_vdiv;
typedef struct _tm_hdiv tm_hdiv;

/* A box filled with a specified character */
typedef struct _tm_debug_box tm_debug_box;

/* A union that represents a color.*/
typedef union tm_color tm_color;

/* A colored char. It has foreground and background color. It also supports transparency (for the background).
 * Any non-zero value as the alpha channel will draw the default background color which is transparent if you have a transparent terminal. */
typedef struct tm_colored_char tm_colored_char;

/* A colored unicode char. It has foreground and background color. It also supports transparency (for the background).
 * Any non-zero value as the alpha channel will draw the default background color which is transparent if you have a transparent terminal. */
typedef struct tm_colored_uchar tm_colored_uchar;

extern bool tm_run;
extern tm_composable* tm_root;

/******************** Struct generator functions *********************/

/* Division generators.
 * Pass two composable which should be in the divisions.
 * The first argument is the left/top composable and the second is the right/bottom*/
tm_vdiv* tm_make_vdiv(tm_composable*, tm_composable*);
tm_hdiv* tm_make_hdiv(tm_composable*, tm_composable*);

/* Debugbox generator. 
 * Pass it the character you want to fill it with */
tm_debug_box* tm_make_debug_box(char c);

/****************** Composition related functions ********************/
/* Set the active root to new_root.
 * Beware! If you did not create new_root with tm_make_root make sure that it doesn't have any NULL pointers (except for focus and lose_focus) or else you will get a segfault.
 * It is assumed that the root has all of its functions defined. */
void tm_set_root(tm_composable* new_root);

/* A function that renders a composeable and its full component tree */
void tm_render(tm_composable*);

/* Compose two UI elements */
tm_composable* tm_compose(tm_composable* element1, tm_composable* element2);


/******************* High level API *****************/

/* If you let the library handle the terminal IO for you this is the API that you get: */
#if TM_HANDLE_MAIN

/* This function is called by the main before starting the main loop.
 * You MUST declare this or you will get a linking error.
 * In this function you should:
 *	- Set keybinds 
 *	- Assign callbacls 
 *	- Initialize values
 *	- etc. */
void tm_init_call();

/* Bind keys to an ASCII value. 
 * Your function must have the signature void(void) to be bindable here.
 * Other than that you can bind to any ASCII keys from the keyboard. */
void tm_bindKey(const char character, void(*function)(void));

/* Bind a callback for every key.
 * When any key is pressed the given function will be called and it will be passed the pressed ASCII character. */
void tm_bind_any_keypress(void(*function)(char));
/* Set a callback for when the terminal is resized.
 * To the given function the new number of rows and columns is passed . */
void tm_set_resize_callback(void(*function)(uint16_t new_width, uint16_t new_height));


/* Once this is called the main loop will exit and thus the program will terminate */
void tm_stop();

#endif


/* Create a color from either rgba values or a hex color (of format #number or just number for rgb and a for alpha) */
tm_color tm_create_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
tm_color tm_create_hex_color(char* s, uint8_t a);

/********************* Mid level API **************************/
/* Most of these functions (and structs) are not necessary to use if you do not descend from the high level api to the mid level. */

/* Create a unicode or ASCII colored character respectively */
tm_colored_char tm_create_colored_char(tm_color fg, tm_color bg, char c);
tm_colored_uchar tm_create_colored_uchar(tm_color fg, tm_color bg, char* code);

/********************* Structs ********************/

/* A struct that represents a color */
struct tm_rgb_color{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

/* A union that represents a color. This is used as tm_color */
union tm_color{
	struct tm_rgb_color channels;
	uint32_t raw;
};

/* A colored char. It has foreground and background color. It also supports transparency (for the background).
 * Any non-zero value as the alpha channel will draw the default background color which is transparent if you have a transparent terminal. */
struct tm_colored_char{
	tm_color fg;
	tm_color bg;
	char ch;
};

/* A colored unicode char. It has foreground and background color. It also supports transparency (for the background).
 * Any non-zero value as the alpha channel will draw the default background color which is transparent if you have a transparent terminal. */
struct tm_colored_uchar{
	tm_color fg;
	tm_color bg;
	char* ch;
};

/* Some macro abuse for more "readability" */
#pragma push_macro("self")
#define self tm_composable*
struct _tm_composable{
	uint16_t x, y; /* Position */
	uint16_t w, h; /* Dimensions */

	tm_composable* inner; /* The element which is composed with this */
	/* Partial of full redraw depending on the module. Now size and position updating. */
	void (*render)(self);
	/* Full redraw and dimensions and position updating */
	void (*rerender)(self);

	void (*focus)(self);
	void (*lose_focus)(self);

	bool (*nav_up)(self);
	bool (*nav_down)(self);
	bool (*nav_left)(self);
	bool (*nav_right)(self);
	bool (*nav_sel)(self);

	bool (*focus_up)(self);
	bool (*focus_down)(self);
	bool (*focus_left)(self);
	bool (*focus_right)(self);
};

#pragma pop_macro("self")

/* Vertical division */
struct _tm_vdiv {
	tm_composable composable;

	/* This inner is the second subdivision. The first is inside the field 'inner'
	 * inside the composable */
	tm_composable* inner;
	uint8_t focused_element; /* This is 1 if the first inner is focused and 2 if the inner inside the composable. (left, right) Otherwise 0. */
};

/* Horizontal division */
struct _tm_hdiv{
	tm_composable composable;
	
	tm_composable* inner; /* This inner is the second subdivision. The first is inside the field 'inner' inside the composable */
	uint8_t focused_element; /* This is 1 if the first inner is focused and 2 if the inner inside the composable. (up, down) Otherwise 0. */
};

struct _tm_debug_box{
	tm_composable composable;
	char c;
};

struct _tm_list{
	tm_composable composable;
	uint16_t length;
	char** contents;
	void (**functions)(void); /* The function for when an option is selected and launched*/
};

/******************************* Low level API ***************************************/
/* You shouldn't have to work with most of these unless you decide to descend to this
 * level of abstraction from the mid and high levels.                                */

/* A simple initialization function for the library.
* Should always be run if you implement the main function. */
void tm_init();

/* A simple ending function for the library.
* Should always be run after the program is ready to stop if you are implementing main. */
void tm_end();

/* Print a colored char */
void tm_print_colored_char(tm_colored_char colored_character);

/* Print a colored unicode char */
void tm_print_colored_uchar(tm_colored_uchar c);

/* Write to the console the requiered escape sequence to switch text color to the given RGB value */
void tm_set_fg(tm_color color);
/* Write to the console the requiered escape sequence to switch background color to the given RGB value */
void tm_set_bg(tm_color color);


/* Reset the forefround and background color to the default. */
void tm_reset_color();

/* Move the cursor to the given position. */
void tm_move_cursor(int row, int column);

/* Find out the size of the terminal.
 * It will write the results into the given variables */
void tm_get_terminal_size(uint16_t* rows, uint16_t* columns);

/* Clear the terminal. */
void tm_clear();

/* Wait for the specified amount of seconds.*/
void tm_wait(unsigned int t);

/* Wait for the specified amount of microseconds.
 * Not absolutely precise. May wait a little more.*/
void tm_waitus(unsigned int t);

#endif
