#include "terminal_manager.h"
#include <stdio.h>

void quit(){
	tm_run = 0;
}

void test(){
	printf("tested\n");
}
void resize(int r, int c){
	printf("resized to %dx%d\n", r, c);
}

void initCall(){
	int r, c;
	getTerminalSize(&r, &c);
	printf("%d %d", r, c);
	if(r < 5 || c < 5){
		printf("Resize the terminal to a more reasonable size!");
	}else{
		clearTerminal();
		int i;
		for(i = 0; i < r*c; i++)
			printf(" ");
	}
	wait(3);
	moveTerminalCursorTo(0,0);
	wait(1);
	int i;
	for(i = 0; i < r*c; i++)
		printf("_");
	setResizeCallback(&resize);
	setCharCallback('q', &quit);
	setCharCallback('t', &test);

}
