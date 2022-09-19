#include "terminal_manager.h"
#include "terminal_manager_low_level.h"
#include "tm_elements.h"
#include <stdio.h>
#include <stdlib.h>
#include "benutils/unicode.h"

void quit(){
	tm_run = 0;
}


char a[] = "asd";
char b[] = "Hello";
char c[] = "there!";

char* l[] = {&a[0], &b[0], &c[0]};

tm_list list;


tm_drawbox db;


void test(){
	tm_render(&db);
};

void resize(int r, int c){
	tm_clear();
	printf("resized to %dx%d\n", r, c);
}

void test2(){
	tm_print_colored_char((tm_colored_char){ (tm_color){125, 125, 125, 0}, (tm_color){0, 0, 0, 0}, 'W' });
	tm_print_colored_char((tm_colored_char){ (tm_color){125, 125, 125, 0}, (tm_color){0, 0, 0, 1}, 'W' });
}

void up(){
	db.on_up(&db);
	tm_render(&db);
}

void down(){
	db.on_down(&db);
	tm_render(&db);
}

void o1(void){
	printf("It works.");
}

void run(){
	db.on_run(&db);
}

void tm_initCall(){
	list = tm_create_list(3, false,
	
	(tm_color){200, 200, 200, 1}, (tm_color){125, 125, 125, 1},
	(tm_color){255, 255, 255, 0}, (tm_color){50, 50, 50, 0},
	&a, NULL, 
	&b, &o1,
	&c, NULL);
	
	db = tm_create_drawbox(1, 1, 12, 12);
	tm_assign_list(&db, &list);
	
	tm_setResizeCallback(&resize);
	tm_bindKey('q', &quit);
	tm_bindKey('t', &test);
	tm_bindKey('r', &run);
	tm_bindKey('i', &up);
	tm_bindKey('k', &down);
}
