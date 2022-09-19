#include <tm_elements.h>
#include <terminal_manager.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/****************** Drawbox related implementations **********************/

tm_drawbox tm_create_drawbox(uint16_t xloc, uint16_t yloc, uint16_t xsize, uint16_t ysize){
	
	struct tm_char_drawbox_field* contents = malloc( xsize * ysize * sizeof(struct tm_char_drawbox_field) );
	
	if(contents == NULL){
		BU_ERR_FUNC("Memory could not be allocated.")
		return (tm_drawbox){0};
	}

	tm_drawbox db;
	
	db.x = xloc;
	db.y = yloc;
	db.x_size = xsize;
	db.y_size = ysize;
	db.contents = contents;

	return db;
}

void tm_render(tm_drawbox* db){
	uint16_t i, j, previ;
	for(i = 0; i < db->y_size; ++i){
		tm_move_cursor(db->y + i + 1, db->x + 1);
		for(j = 0; j < db->x_size; ++j){
			if(db->contents[i*db->x_size + j].modified){
				tm_print_colored_char( db->contents[i*db->x_size + j].cch );
			}
		}
	}

}

void tm_draw(tm_drawbox* db){
	db->draw(db);
}

void tm_assign(tm_drawbox* db,  void* data, void (*draw)(tm_drawbox*),
	void (*on_focus)(tm_drawbox*),	void (*on_down)(tm_drawbox*),
	void (*on_up)(tm_drawbox*),   	void (*on_left)(tm_drawbox*),
	void (*on_right)(tm_drawbox*),	void (*on_run)(tm_drawbox*),
	void (*focus_lost)(tm_drawbox*) ){
	
	db->data = data;
	db->draw = draw;
	db->on_focus = on_focus;
	db->on_down = on_down;
	db->on_up = on_up;
	db->on_left = on_left;
	db->on_right = on_right;
	db->on_run = on_run;
	db->focus_lost = focus_lost;
}

/**************** List related implementations ***********************/

tm_list tm_create_list(uint16_t length, bool focuseable, tm_color fg, tm_color bg, tm_color ffg, tm_color fbg, ...){
	tm_list list;
	char** entries = malloc( length*sizeof(tm_colored_char) );
	void (**f)(void) = malloc(length*sizeof(void (*)(void) ) );

	if(entries == NULL || f == NULL){
		BU_ERR_FUNC("Memory could not be allocated.")
		return (tm_list){0};
	}

	va_list args;
	va_start(args, fbg);
	
	int i;
	for(i = 0; i < length; ++i){
		entries[i] = va_arg(args, char*);
		f[i] = va_arg(args, void (*)(void));
	}

	list.length = length;
	list.focuseable = focuseable;
	list.fg = fg;
	list.bg = bg;
	list.ffg = ffg;
	list.fbg = fbg;
	list.entries = &entries[0];
	list.f = f;

	list.focused_element = 0;
	
	return list;
}

void tm_list_draw(tm_drawbox* db){
	tm_list* list = db->data;
	uint16_t i = 0;
	for(; (i < list->length) && (i < db->y_size); ++i){
		int len = strlen( list->entries[i] );
		int j;
		for(j = 0; j < db->x_size; ++j){
			if(j < len){
				if(db->contents[i*db->x_size + j].cch.ch != list->entries[i][j]){
					db->contents[i*db->x_size + j].cch.ch = list->entries[i][j];
					db->contents[i*db->x_size + j].modified = true;				
				}
			}else if(db->contents[i*db->x_size + j].cch.ch != ' '){
				db->contents[i*db->x_size + j].cch.ch = ' ';
				db->contents[i*db->x_size + j].modified = true;				
			}

			if(i == list->focused_element - 1 &&
			  ( db->contents[i*db->x_size + j].cch.fg.raw != list->ffg.raw
			  || db->contents[i*db->x_size + j].cch.bg.raw != list->fbg.raw) )
			{
				db->contents[i*db->x_size + j].cch.fg = list->ffg;
				db->contents[i*db->x_size + j].cch.bg = list->fbg;
				db->contents[i*db->x_size + j].modified = true;

			}
			if(i != list->focused_element - 1 &&
			 (  db->contents[i*db->x_size + j].cch.fg.raw != list->fg.raw
			 || db->contents[i*db->x_size + j].cch.bg.raw != list->bg.raw) ){
				
				db->contents[i*db->x_size + j].cch.fg = list->fg;
				db->contents[i*db->x_size + j].cch.bg = list->bg;
				db->contents[i*db->x_size + j].modified = true;
			}

		}
	}
}

void tm_list_on_focus(tm_drawbox* db){
	tm_list* list = db->data;
	list->focused_element = 1;
}

void tm_list_on_down(tm_drawbox* db){
	// Grab the list from the drawbox
	tm_list* list = db->data;
	if(list->focused_element + 1 <= list->length)
		list->focused_element++;
	//else
	db->draw(db);
}

void tm_list_on_up(tm_drawbox* db){
	tm_list* list = db->data;
	if(list->focused_element - 1 > 0)
		list->focused_element--;
	db->draw(db);
}


void tm_list_on_left(tm_drawbox* db){
	
}

void tm_list_on_right(tm_drawbox* db){

}

void tm_list_on_run(tm_drawbox* db){
	tm_list* list = db->data;
	if(list->f[list->focused_element - 1] != NULL)
		list->f[list->focused_element - 1]();
}

void tm_list_focus_lost(tm_drawbox* db){
	tm_list* list = db->data;
	list->focused_element = 0;
	db->draw(db);
}

void tm_assign_list(tm_drawbox* db, tm_list* data){
	tm_assign(db, data, &tm_list_draw,
			&tm_list_on_focus,	&tm_list_on_down,
			&tm_list_on_up,   	&tm_list_on_left,
			&tm_list_on_right,	&tm_list_on_run,
			&tm_list_focus_lost);
	db->data = data;
}

