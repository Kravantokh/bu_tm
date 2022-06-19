/** \file tm_elements.h
* \brief Currently unused. Reserved for future development.
*/
#ifndef __BU_TERMINAL_MANAGER_ELEMENTS__
#define __BU_TERMINAL_MANAGER_ELEMENTS__

#include <terminal_manager.h>
#include <stdint.h>


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
