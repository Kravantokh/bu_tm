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



/* Drawbox - technically a fancy buffer which represents an area of the terminal in memory.
 * This is the most important element of the high level API.
 * Elements such as lists can be associated to a drawbox.
 *
 * This association means that from that point on the drawbox's contents will be filled with
 * the associated element.
 *
 * Selective refreshing works automatically with all the built-in element types
 *
 * See tm_create_drawbox for creating one. */
typedef struct _tm_drawbox tm_drawbox;

/* A struct which represents a list. This can be assigned to a drawbox in order to make it a list.
 * It can run a function on every one of its elements. It can also highlight the focused element.
 * See tm_create_list for creating one.
 * Simple text can be drawn as an unfocusable list of a single element. */
 typedef struct _tm_list tm_list;

/* Representation of the layout of the app.
 * This is used to automatically switch between UI elements. */
typedef struct _tm_layout tm_layout;
typedef struct _tm_layout_element tm_layout_element;

/* A union that represents a color.*/
typedef union tm_color tm_color;

/* A colored char. It has foreground and background color. It also supports transparency (for the background).
 * Any non-zero value as the alpha channel will draw the default background color which is transparent if you have a transparent terminal. */
typedef struct tm_colored_char tm_colored_char;


/* A colored unicode char. It has foreground and background color. It also supports transparency (for the background).
 * Any non-zero value as the alpha channel will draw the default background color which is transparent if you have a transparent terminal. */
typedef struct tm_colored_uchar tm_colored_uchar;


extern tm_layout* tm_global_layout;
extern bool tm_run;

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
void tm_initCall();

/* Bind keys to an ASCII value. 
 * Your function must have the signature void(void) to be bindable here.
 * Other than that you can bind to any ASCII keys from the keyboard. */
void tm_bindKey(const char character, void(*function)(void));

/* Bind a callback for every key.
 * When any key is pressed the given function will be called and it will be passed the pressed ASCII character. */
void tm_bindToAnyKeypress(void(*function)(char));

/* Set a callback for when the terminal is resized.
 * To the given function the new number of rows and columns is passed . */
void tm_setResizeCallback(void(*function)(int, int));

/* Once this is called the main loop will exit and thus the program will terminate */
void tm_stop();

#endif


/* Create a layout.
 * A layout holds all the drawboxes shown on the screen.
 * With the help of the layout manager it can be used to manage navigation between elements. */
tm_layout tm_create_layout(uint8_t width, uint8_t height);

/* Sets an element in a layout to the given element (drawbox).
 * x is horizontal and y is vertical (it increases downwards). Both start from 0. */
void tm_set_layout_element(tm_layout* layout, uint8_t x_index, uint8_t y_index, tm_drawbox* const element, float width, float height);

/* Set the global layout (layout shown on the screen) to a given layout*/
void tm_activate_layout(tm_layout* const layout);

/* Render a given layout to the screen */
void tm_update_layout(tm_layout* layout);

/* Delete a layout
 * Frees the associated memory. No need for more explicit manual cleanup.*/
void tm_delete_layout(tm_layout layout);


 /* Create a drawbox. */
tm_drawbox tm_create_drawbox(uint16_t xloc, uint16_t yloc, uint16_t xsize, uint16_t ysize);

/* Ceate a list made up of the given entries and their associated functions.
 * Put your entries and their functions of signature void(void) in place of the ... when calling this function.
 * Pay attention to specify the correct length as the first argument (It must be exactly the number of entries)
 * otherwise the call may result in undefined behaviour (a segfault in the best case). 
 * The length specifies the number of entry and function pairs.
 * If you do not want to associate a function to an entry you may pass null.
 * Also see the example high_level_api.c */
tm_list tm_create_list(uint16_t length, bool focuseable, tm_color fg, tm_color bg, tm_color ffg, tm_color fbg, ...);

/* Assign a list to a drawbox. After using this that drawbox will become the given list. */
void tm_assign_list(tm_drawbox* db, tm_list* data);

/* Create a color from either rgba values or a hex color (of format #number or just number for rgb and a for alpha) */
tm_color tm_create_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
tm_color tm_create_hex_color(char* s, uint8_t a);

/********************* Mid level API **************************/
/* Most of these functions (and structs) are not necessary to use if you do not descend from the high level api to the mid level. */

/* Draw a drwabox's contents to the screen*/
void tm_render(tm_drawbox* db);

/* Refresh the internal memory of a drawbox.
 * To draw to the screen use tm_render instead. */
void tm_draw(tm_drawbox* drawbox);

/* Assign a draw function and drawing data to a drawbox.
 * Unless you are implementing a custom element you should have nothing to do with this. */
void tm_assign(tm_drawbox* db,  void* data, void (*draw)(tm_drawbox*),
	void (*on_focus)(tm_drawbox*),	void (*on_down)(tm_drawbox*),
	void (*on_up)(tm_drawbox*),   	void (*on_left)(tm_drawbox*),
	void (*on_right)(tm_drawbox*),	void (*on_run)(tm_drawbox*),
	void (*focus_lost)(tm_drawbox*)
);

/* Function used to resize drawboxes (No need to use this if you use the layout manager) */
void tm_resize_drawbox(tm_drawbox*, uint16_t newx, uint16_t newy);


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


/* Representation of a layout. */
struct _tm_layout{
	uint8_t width;
	uint8_t height;
	/* 2D array indexed as 1D*/
	tm_layout_element* elements;
};

struct _tm_layout_element{
	tm_drawbox* db;
	// Relative dimensions compared to the whole window.
	float width;
	float height;
};

// This is an element of a drawbox. The modified boolean is for selective redrawing. Normally you shouldn't have anything to do with this. It is meant to be abstracted away by higher level functions.
struct tm_char_drawbox_field{
	tm_colored_char cch;
	bool modified;
};

// Drawbox. Technically a fancy buffer, which stores characters and their colors along with its dimensions and position on the screen. It also stores some functions which are used for refreshing its state and the state of the program.
struct _tm_drawbox{
	// Position
	uint16_t x, y;
	// Dimensions
	uint16_t x_size, y_size;
	// Contents (A 2D array of fields stored as a 1D array, indexed with x + y*x_size)
	
	struct tm_char_drawbox_field* contents;
	// Extra data used by the drawing function
	void* data;
	
	// Drawing function
	void (*draw)(struct _tm_drawbox* self);
	// Focus action
	void (*on_focus)(struct _tm_drawbox* self);
	// Navigation down action
	void (*on_down)(struct _tm_drawbox* self);
	// Navigation up action
	void (*on_up)(struct _tm_drawbox* self);
	// Navigation left action
	void (*on_left)(struct _tm_drawbox* self);
	// Navigation right action
	void (*on_right)(struct _tm_drawbox* self);
	// Main action (Called when enter is pressed)
	void (*on_run)(struct _tm_drawbox* self);
	// Action on focus lost
	void (*focus_lost)(struct _tm_drawbox* self);

};


struct _tm_list{
	// Number of entries
	uint16_t length;
	// Pointer to the entries.
	char** entries;
	// Function pointers to the actions performed on every option.
	void (**f)(void);
	// Focused elements' bg and fg are automatically inverted
	bool focuseable;
	uint16_t focused_element;
	// Normal colors
	tm_color fg;
	tm_color bg;
	// Focused colors
	tm_color ffg;
	tm_color fbg;

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
void tm_get_terminal_size(int* rows, int* columns);

/* Clear the terminal. */
void tm_clear();

/* Wait for the specified amount of seconds.*/
void tm_wait(unsigned int t);

/* Wait for the specified amount of microseconds.
 * Not absolutely precise. May wait a little more.*/
void tm_waitus(unsigned int t);

#endif
