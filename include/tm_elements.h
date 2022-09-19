#ifndef __BU_TERMINAL_MANAGER_ELEMENTS__
#define __BU_TERMINAL_MANAGER_ELEMENTS__

#include "terminal_manager.h"
#include "terminal_manager_low_level.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* A global error function which you can replace with a custom function */
#ifndef BU_ERR_FUNC
#define BU_ERR_FUNC(E) fprintf(stderr, E);
#endif

/************* Struct forward declarations and globals ************/
/* This section is meant to be short and remove the clutter caused by all the structs upfront.
 * Most of these structs are used in the mid level API anyways, but
 * a short description of their roles will pe provided here. */


/* Technically a fancy buffer which represents an area of the terminal in memory.
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


/******************* High level API *****************/


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


/********************* Mid level API **************************/
/* Most of these functions (and structs) are not necessary to use if you do not descend from the high level api to the mid level. */


/* Draw a drwabox's contents to the screen*/
void tm_render(tm_drawbox* db);

/* Refresh the internal memory of a drawbox.
 * To draw to the screen use tm_render instead. */
void tm_draw(tm_drawbox* drawbox);

/*
 * Assign a draw function and drawing data to a drawbox.
 * Unless you are implementing a custom element you should have nothing to do with this.
*/
void tm_assign(tm_drawbox* db,  void* data, void (*draw)(tm_drawbox*),
	void (*on_focus)(tm_drawbox*),	void (*on_down)(tm_drawbox*),
	void (*on_up)(tm_drawbox*),   	void (*on_left)(tm_drawbox*),
	void (*on_right)(tm_drawbox*),	void (*on_run)(tm_drawbox*),
	void (*focus_lost)(tm_drawbox*)
);

// Function used to resize drawboxes (No need to use this if you use the layout manager)
void tm_resize_drawbox(tm_drawbox*, uint16_t newx, uint16_t newy);


/********************* Structs ********************/


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
	// Extra data used by the drawing function
	void *data;
	// Contents (A 2D array of fields stored as a 1D array, indexed with x + y*x_size)
	struct tm_char_drawbox_field* contents;
	
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
#endif
