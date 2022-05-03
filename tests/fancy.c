#include "terminal_manager.h"
#include <stdio.h>

void quit(){
	tm_run = 0;
	tm_clear();
}

void test(){
	int r, c;
	tm_getTerminalSize(&r, &c);
	printf("%d %d\n", r, c);
}
void resize(int r, int c){
	tm_clear();
	printf("resized to %dx%d\n", r, c);
}

void initCall(){
	int r, c;
	tm_getTerminalSize(&r, &c);
	printf("%d %d", r, c);
	int i;
	for(i = 0; i<r*c; ++i)
		printf(" ");
	tm_setResizeCallback(&resize);
	tm_bindKey('q', &quit);
	tm_bindKey('t', &test);
}

void tm_initCall(){
	printf("Init");
	tm_setResizeCallback(&resize);
	tm_bindKey('q', &quit);
	tm_bindKey('t', &test);
}
