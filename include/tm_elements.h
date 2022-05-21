/** \file tm_elements.h
* \brief Currently unused. Reserved for future development.
*/
#ifndef __BU_TERMINAL_MANAGER_ELEMENTS__
#define __BU_TERMINAL_MANAGER_ELEMENTS__

#include <terminal_manager.h>
#include <stdint.h>

/** \brief A buffer to store a number of characters. Its contents are dynamically allocated.
 * \param x, y - dimensions of the buffer
 * \param contents
*/
struct tm_char_buffer{
	int x, y;
	char* contents[];
};

typedef struct tm_char_buffer tm_char_buffer;

tm_init_char_buffer(tm_char_buffer buff){
	buff.x = 0;
	buff.y = 0;
	contents = 0;
}

/** \brief A function to allocate a \link tm_char_buffer tm_char_buffer\endlink struct of the given dimensions.
*/
tm_char_buffer* tm_allocate_char_buffer(int x, int y){
	tm_char_buffer buff = {x, y, 0};
	buff.contents = malloc
	return buff;
}

void tm_deallocate_char_buffer(tm_char_buffer* buff){
	if(buff.x != 0 && buff.y != 0){
		if(contents != 0){
			int i;
			for(int i = 0; i < x*y; ++i)
				free()
		}
}

struct tm_element{
	uint8_t xsize, ysize;

};

char tm_listPrintTo(uint8_t x, uint8_t y);

void tm_initList();

typedef struct tm_element tm_element;


/*
int tm_getBit(uint8_t variable, uint8_t index);

void tm_setBit(uint8_t* variable, uint8_t index, uint8_t value);
*/

#endif
