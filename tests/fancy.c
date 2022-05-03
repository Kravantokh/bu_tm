#include "terminal_manager.h"
#include <stdio.h>
#include <stdlib.h>

void quit(){
	tm_run = 0;
	tm_clear();
}

void test(){


       if (printf("%lc\n", 0x2588) < 0) {
           perror("printf");
       }

}
void resize(int r, int c){
	tm_clear();
	printf("resized to %dx%d\n", r, c);
}

void tm_initCall(){
	/*tm_colored_char c1 = {125,125,125,0, 255,255,255,0, 'c'};*/
	tm_setResizeCallback(&resize);
	tm_bindKey('q', &quit);
	tm_bindKey('t', &test);
}
