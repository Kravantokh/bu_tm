#include <tm_elements.h>
#include <terminal_manager.h>


void tm_set_layout(uint8_t x, uint8_t y){
	tm_layoutX = x;
	tm_layoutY = y;

}

void tm_render(int nrows, int ncols){

}

void tm_rendererInit(){
	tm_setResizeCallback(&tm_render);

}


int tm_getBit(uint8_t variable, uint8_t index){
	return (variable >> index) & 1;
}

void tm_setBit(uint8_t* variable, uint8_t index, uint8_t value){
	if (value == 0)
		*variable &= (0xFF & (0 << index));
	if (value == 1)
		(*variable) |= (0x00 | (1 << index));
}
