#include "terminal_manager.h"

tm_list list1;

tm_drawbox db1;
tm_drawbox db2;

tm_layout l;
char a[] = "asd";
char b[] = "Hello";
char c[] = "there!";

void o1(void){
	printf("It works.");
}

void quit(){
	tm_run = false;
}

void on_resize(){
	tm_clear();
	tm_update_layout(&l);
	tm_render_layout(&l);
}

void tm_init_call(){
	tm_bindKey('q', &quit);
	tm_setResizeCallback(&on_resize);

	db1 = tm_create_drawbox(0, 0, 10, 20);
	db2 = tm_create_drawbox(11, 0, 10, 20);
	
	list1 = tm_create_list(3, 
	(tm_color){200, 200, 200, 1}, (tm_color){125, 125, 125, 1},
	(tm_color){255, 255, 255, 0}, (tm_color){50, 50, 50, 0},
	&a, NULL, 
	&b, &o1,
	&c, NULL);
	
	tm_assign_list(&db1, &list1);
	tm_assign_list(&db2, &list1);

	tm_resize_drawbox(&db1, 20, 30);

	l = tm_create_layout(2, 1);

	tm_set_layout_element(&l, 0, 0, &db2, 0.5, 1);
	tm_set_layout_element(&l, 1, 0, &db1, 0.5, 1);

	tm_activate_layout(&l);
	
	tm_update_layout(&l);
	tm_render_layout(&l);
	
}
