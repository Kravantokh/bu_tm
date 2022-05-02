#include "terminal_manager.h"
#include <stdio.h>

void quit(){
	tm_run = 0;
	clearTerminal();
}

void test(){
	int r, c;
	getTerminalSize(&r, &c);
	printf("%d %d\n", r, c);
}
void resize(int r, int c){
	printf("resized to %dx%d\n", r, c);
}

void initCall(){
	int r, c;
	getTerminalSize(&r, &c);
	printf("%d %d", r, c);
	int i;
	for(i = 0; i<r*c; ++i)
		printf(" ");
	setResizeCallback(&resize);
	setCharCallback('q', &quit);
	setCharCallback('t', &test);

}
