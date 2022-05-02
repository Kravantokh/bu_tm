/** \file tm_elements.h
* \brief Currently unused. Reserved for future development.
*/
#ifndef __BU_TERMINAL_MANAGER_ELEMENTS__
#define __BU_TERMINAL_MANAGER_ELEMENTS__

#include <terminal_manager.h>
#include <stdint.h>

#ifndef TM_MAX_ELEMENT_NUM
	#define TM_MAX_ELEMENT_NUM 100
#endif

/**/


/* \brief !!CURRENTLY UNFINISHED!! Represents possible UI elements.
* Those elements can be lists, textboxes, drawboxes etc.
*/

/*struct tm_element{
	char (*printTo)(uint8_t, uint8_t);
	uint8_t x,y;
	uint8_t xsize, ysize;

};*/

char tm_listPrintTo(uint8_t x, uint8_t y);

void tm_initList();
/*Element types are:


*/

/*Element types -1 and 0 are reserved*/
/*
struct tm_textBox{
	struct tm_element;
	char* contents;
};


struct tm_elementList{
	struct tm_element;
	struct tm_element* contents;
};


struct tm_drawBox{
	struct tm_element;
	char** contents;
};


struct tm_button{
	struct tm_element;
	char* contents;
};
*/
typedef struct tm_element tm_element;/*
typedef struct tm_elementList tm_elementList;
typedef struct tm_drawBox tm_drawBox;*/


/*
int tm_getBit(uint8_t variable, uint8_t index);

void tm_setBit(uint8_t* variable, uint8_t index, uint8_t value);
*/

#endif
