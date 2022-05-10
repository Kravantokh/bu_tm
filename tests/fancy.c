#include "terminal_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include "benutils/unicode.h"

void quit(){
	tm_run = 0;
}

void test(){
/*	printf("%c", 226);
	printf("%c", 152);
	printf("%c \n ", 131);
	char buf[] = { 226, 152 , 131 };
*/
	tm_color c1 = tm_create_color(125,125,125,0);
	tm_color c2 = tm_create_color(255,255,255,0);
	tm_resetColor();

	char* c = encode_uchar("U+00BF");
	print_uchar(c);


}
void resize(int r, int c){
	tm_clear();
	printf("resized to %dx%d\n", r, c);
}

void any(char ch){
	printf("%c", ch);
}

void tm_initCall(){
	/*tm_colored_char c1 = {125,125,125,0, 255,255,255,0, 'c'};*/
	tm_setResizeCallback(&resize);
	tm_bindKey('q', &quit);
	tm_bindKey('t', &test);
	/*tm_bindToAnyKeypress(&any);*/
}
