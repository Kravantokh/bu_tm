#include "terminal_manager.h"
#include <stdio.h>

void quit(){
	tm_run = 0;
}

void test(){
	printf("tested\n");
}
void resize(int r, int c){
	tm_clear();
	printf("resized to %dx%d\n", r, c);
}


void tm_initCall(){
	printf("Init");
	setResizeCallback(&resize);
	tm_bindKey('q', &quit);
	tm_bindKey('t', &test);
}
