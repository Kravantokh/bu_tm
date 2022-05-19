#include "terminal_manager.h"
#include "terminal_manager_low_level.h"
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
	tm_color c1 = tm_create_hex_color("#7D7D7D", 0);
	tm_color c2 = tm_create_color(255,255,255,0);
	tm_reset_color();
	char* c = encode_uchar("U+0074");
	tm_colored_uchar cc = tm_create_colored_uchar(c1, c2, c);
	tm_print_colored_uchar(cc);
	tm_colored_char ccc = tm_create_colored_char(c1, c2, 'y');
	tm_print_colored_char(ccc);
}


void test2(){
/*	printf("%c", 226);
	printf("%c", 152);
	printf("%c \n ", 131);
	char buf[] = { 226, 152 , 131 };
*/
	/*tm_color c1 = tm_create_color(125,125,125,0);
	tm_color c2 = tm_create_color(255,255,255,0);*/
	tm_reset_color();

	char* c = encode_uchar("U+0072");
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
	tm_bindKey('r', &test);
	tm_bindKey('t', &test2);
	/*tm_bindToAnyKeypress(&any);*/
}
