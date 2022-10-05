#include "terminal_manager.h"

char a[] = "asd";
char b[] = "Hello";
char c[] = "there!";

void o1(void){
	printf("It works.");
}

void quit(){
	tm_run = false;
}

void up(){
	tm_root->focus_up(tm_root);
}
void down(){
	tm_root->focus_down(tm_root);
}
void left(){
	tm_root->focus_left(tm_root);
}
void right(){
	tm_root->focus_right(tm_root);
}

void on_resize(uint16_t w, uint16_t h){
	tm_clear();
	tm_root->w = w;
	tm_root->h = h;
	tm_root->rerender(tm_root);
}

void pr(char c){
	printf("%d\n", c);
}

void tm_init_call(){
	tm_bindKey('q', &quit);
	tm_set_resize_callback(&on_resize);
	/* tm_bind_any_keypress(&pr); */
	tm_bindKey('k', &up);
	tm_bindKey('j', &down);
	tm_bindKey('l', &right);
	tm_bindKey('h', &left);
	tm_set_root(
			(void*)tm_make_vdiv(
				(void*)tm_make_vdiv(
					(void*)tm_make_debug_box('3'),
					/*(void*)tm_make_vdiv(
						(void*)tm_make_debug_box('1'),
						(void*)tm_make_debug_box('2')
					)*/
					(void*)tm_make_debug_box('4')
				),
				(void*)tm_make_debug_box('@')
			)
	);
	tm_clear();
	tm_get_terminal_size(&tm_root->w, &tm_root->h);
	tm_root->rerender(tm_root);
	tm_root->render(tm_root);
}
