#ifdef __linux__
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <termios.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <time.h>
#elif _WIN32
	#include <windows.h>
#else
	#error Unsupported platform.
#endif

#include <stdio.h>
#include <locale.h>
#include <stdarg.h>
#include "terminal_manager.h"
#include "benutils/unicode.h"

#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO tm_csbi;
	HANDLE tm_inputHandle;
	HANDLE tm_outputHandle;
	DWORD oldMode, newMode;
	struct _CONSOLE_FONT_INFOEX old_info = {sizeof(CONSOLE_FONT_INFOEX)}, new_info = {sizeof(CONSOLE_FONT_INFOEX)};

#elif __linux__
	struct winsize tm_winsize;
	static struct termios oldt, newt;
	struct timeval tv;
	fd_set rfds;
#endif
/********************** Global variable initializations ************************/

#if TM_HANDLE_MAIN
/* A pointer array for all the possible pressable ASCII keys.
This will be used to call the callbacks for each key. */
void (*tm_key_callbacks[128])();
void (*tm_resize_callback)(int, int);
void (*tm_any_char_callback)(char);
#endif


bool tm_run = 1;
tm_layout* tm_global_layout;

tm_color tm_cur_fg = (tm_color){0, 0, 0, 0};
tm_color tm_cur_bg = (tm_color){0, 0, 0, 0};

/*********************** Struct manager functions *************************/

tm_colored_char tm_create_colored_char(tm_color fg, tm_color bg, char c){
	tm_colored_char coloredChar = {fg, bg, c};
	return coloredChar;
}

tm_colored_uchar tm_create_colored_uchar(tm_color fg, tm_color bg, char* code){
	tm_colored_uchar coloreduChar = {fg, bg, code};
	return coloreduChar;
}

tm_color tm_create_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	tm_color color;
	color.channels = (struct tm_rgb_color){r, g, b, a};
	return color;
}

tm_color tm_create_hex_color(char* val, uint8_t a){
	uint32_t data = 0;

	if(*val == '#')
		sscanf(val, "#%6x", &data);
	else
		sscanf(val, "%6x", &data);

	tm_color color;

	color.channels.blue = 0x00 | data;
	data >>= 8;
	color.channels.green = 0x00 | data;
	data >>= 8;
	color.channels.red = 0x00 | data;
	data >>= 8;
	color.channels.alpha = a;

	return color;
}


/******************** Layout related implementations *******************/

tm_layout tm_create_layout(uint8_t w, uint8_t h){
	
	tm_layout_element* elements = malloc( w*h*sizeof(tm_layout_element) );
	if(elements == NULL){
		BU_ERR_FUNC("Could not allocate memory.\n");
		return (tm_layout){0};
	}
	tm_layout l;
	l.elements = elements;
	l.width = w;
	l.height = h;
	return l;
}

void tm_set_layout_element(tm_layout* layout, uint8_t x_index, uint8_t y_index, tm_drawbox* db, float width, float height){
	if(x_index < layout->width && y_index < layout->height){
		layout->elements[x_index + y_index * layout->width] = (tm_layout_element){db, width, height};
	}
	return;
}

void tm_activate_layout(tm_layout* const layout){
	tm_global_layout = layout;
}


void tm_delete_layout(tm_layout layout){
	free(layout.elements);
	layout.width = 0;
	layout.height = 0;
}

void tm_update_layout(tm_layout* layout){
	int w, h;
	tm_get_terminal_size(&h, &w);
	
	unsigned int i, j=0;
	for(i = 0; i < layout->width; ++i){	
		
		for(j = 0; j < layout->height; ++j){
			tm_layout_element curr_element = layout->elements[i + j * layout->width];
			
			curr_element.db->x_size = w * curr_element.width;
			curr_element.db->y_size = h * curr_element.height;

			if(i == 0 && j==0){
				curr_element.db->x = 0;
				curr_element.db->y = 0;
			}
			
			if(i > 0){
				tm_layout_element previous_horizontal_element = layout->elements[i - 1 + j * layout->width]; 
				curr_element.db->x = previous_horizontal_element.db->x + previous_horizontal_element.db->x_size + 1;
			}
			if(j > 0){
				tm_layout_element previous_vertical_element = layout->elements[i + (j - 1) * layout->width]; 
				curr_element.db->y = previous_vertical_element.db->y + previous_vertical_element.db->y_size + 1;
			}

			/* Resigzing and redrawing */
			tm_resize_drawbox( curr_element.db, curr_element.db->x_size, curr_element.db->y_size);	
			curr_element.db->draw(curr_element.db);
		}

	}

}

void tm_render_layout(tm_layout* l){
	unsigned int i, j;
	for(i = 0; i < l->width; ++i){
		for(j = 0; j < l->height; ++j){
			l->elements[i + j * l->width].db->draw(l->elements[i + j * l->width].db);
			tm_render(l->elements[i + j * l->width].db);
		}
	}
}

/****************** Drawbox related implementations **********************/

tm_drawbox tm_create_drawbox(uint16_t xloc, uint16_t yloc, uint16_t xsize, uint16_t ysize){
	tm_drawbox db;
	
	db.contents = malloc( xsize * ysize * sizeof(struct tm_char_drawbox_field) );
	

	if(db.contents == NULL){
		BU_ERR_FUNC("Memory could not be allocated.")
		return (tm_drawbox){0};
	}

	
	db.x = xloc;
	db.y = yloc;
	db.x_size = xsize;
	db.y_size = ysize;

	return db;
}

void tm_resize_drawbox(tm_drawbox* db, uint16_t newx, uint16_t newy){
	free( db->contents );
	
	db->contents = calloc( newx * newy, sizeof(struct tm_char_drawbox_field) );
	
	if(db->contents == NULL){
		BU_ERR_FUNC("Memory could not be allocated.")
	}

	db->x_size = newx;
	db->y_size = newy;
	db->draw(db);
}


void tm_render(tm_drawbox* db){
	uint16_t i, j, previ;
	for(i = 0; i < db->y_size; ++i){
		tm_move_cursor(db->y + i, db->x);
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
		f[i] = va_arg(args, void(*)(void));
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
/*	
	tm_list* list = db->data;
	uint16_t i = 0, j = 0;
	for(;  (i < db->y_size); ++i){
		for(j = 0; j < db->x_size; ++j){
			db->contents[i*db->x_size + j].cch.ch = (i + j)%10 + 48;
			db->contents[i*db->x_size + j].modified = true;				
		
		}
	}	
*/
	tm_list* list = db->data;
	uint16_t i = 0;
	for(; (i < list->length) && (i < db->y_size - 1); ++i){
		int len = strlen( list->entries[i] );
		int j;
		for(j = 0; j < db->x_size - 1; ++j){
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

/********** Terminal interaction functions **********/

void tm_print_colored_char(tm_colored_char c){
	tm_set_bg(c.bg);
	tm_set_fg(c.fg);
	putchar(c.ch);
}

void tm_print_colored_uchar(tm_colored_uchar c){
	if(c.bg.channels.alpha > 0)
		tm_reset_color();
	else
		tm_set_bg(c.bg);
	tm_set_fg(c.fg);
	print_uchar(c.ch);
}

void tm_get_terminal_size(int* rows, int* columns){
	#ifdef _WIN32
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &tm_csbi);
		*rows = tm_csbi.srWindow.Bottom - tm_csbi.srWindow.Top + 1;
		*columns = tm_csbi.srWindow.Right - tm_csbi.srWindow.Left + 1;
	#elif __linux__
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &tm_winsize);
		*rows = tm_winsize.ws_row;
		*columns = tm_winsize.ws_col;
	#endif
}

void tm_move_cursor(int row, int column){
	printf("\033[%d;%dH", row, column);
}

void tm_clear(){
	#ifdef _WIN32
		system("cls");
	#elif __linux__
		system("clear");
	#endif
}

void tm_reset_color(){
	printf("\x1b[39m");
	printf("\x1b[49m");
}

void tm_set_fg(tm_color color){
	if(tm_cur_fg.raw == color.raw)
		return;
	printf("\x1b[38;2;%u;%u;%um", color.channels.red, color.channels.green, color.channels.blue);
	tm_cur_fg = color;
}

void tm_set_bg(tm_color color){
	if(tm_cur_bg.raw == color.raw)
		return;
	if(color.channels.alpha > 0)
		printf("\x1b[49m");
	else
		printf("\x1b[48;2;%u;%u;%um", color.channels.red, color.channels.green, color.channels.blue);
	tm_cur_bg = color;
}

char tm_getch(){
	#ifdef _WIN32
	DWORD tm_events;
	INPUT_RECORD tm_buffer;
	PeekConsoleInput(tm_inputHandle, &tm_buffer, 1, &tm_events);

	if(tm_events > 0){
		ReadConsoleInput(tm_inputHandle, &tm_buffer, 1, &tm_events);
			if(tm_buffer.EventType == KEY_EVENT)
				if(tm_buffer.Event.KeyEvent.bKeyDown)
					return tm_buffer.Event.KeyEvent.uChar.AsciiChar;

		return 0;
	}
	else return 0;

	#elif __linux__
		char c;
		struct timeval tv = { 0L, 0L };
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(0, &fds);
		int res = 0;
		if(select(1, &fds, NULL, NULL, &tv) > 0)
			res = read(1, &c, sizeof(c));
		if(res <= 0)
			c = 0;
		return c;
	#endif
}

/*********************** Timing functions ********************/

void tm_wait(unsigned int t){
	#ifdef _WIN32
		Sleep(1000 * t); /*from seconds to milliseconds*/
	#elif __linux__
		sleep(t);
	#endif
}

void tm_waitus(unsigned int t){
	#ifdef _WIN32
		/* Apparently windows has no usleep function, so it has to be defined*/
		HANDLE timer;
		LARGE_INTEGER ft;

		ft.QuadPart = -(10 * (__int64)t);

		timer = CreateWaitableTimer(NULL, TRUE, NULL);
		SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
		WaitForSingleObject(timer, INFINITE);
		CloseHandle(timer);
	#elif __linux__
		struct timeval time;
		time.tv_sec = 0;
		time.tv_usec = t;
		/* Use select for wait, because usleep has been deprecated and nanosleep is too new*/
		select(0, 0, 0, 0, &time);
	#endif
}
/*********************** Library init and end ***********************/

void tm_init(){
	#ifdef _WIN32
		/* Get some infom and set handles */
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &tm_csbi);
		tm_outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		tm_inputHandle = GetStdHandle(STD_INPUT_HANDLE);
		DWORD oldMode = 0;

		/* Enable truecolor */
		GetConsoleMode(tm_outputHandle, &oldMode);
		newMode = oldMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(tm_outputHandle, newMode);
		/* Prepare for unicode. */
		setlocale(LC_ALL, "en_US");
		system("chcp 65001");
		tm_clear();
		/* Set a font that supports unicode */
		GetCurrentConsoleFontEx(tm_outputHandle, 0, &old_info);
		new_info = old_info;
		new_info.nFont = 0;
		wcscpy(new_info.FaceName, L"NSimSun"); /*L"MS Gothic"*/
		SetCurrentConsoleFontEx(tm_outputHandle, 0, &new_info);
	#elif __linux__
		tcgetattr( STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &tm_winsize);
		tcsetattr( STDIN_FILENO, TCSANOW, &newt);
	#endif
}

void tm_end(){
	tm_reset_color();
	tm_clear();
	#ifdef _WIN32
	SetConsoleMode(tm_outputHandle, oldMode);
	SetCurrentConsoleFontEx(tm_outputHandle, 0, &old_info);
	#elif __linux__
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
	#endif
}



void tm_store_rgbFG(char* s, int red, int green, int blue){
	sprintf(s, "\x1b[38;2;%d;%d;%dm", red, green, blue);
}

void tm_store_rgbBG(char* s, int red, int green, int blue){
	sprintf(s, "\x1b[48;2;%d;%d;%dm", red, green, blue);
}




#ifdef __cplusplus
char getch(){
	return tm_getch();
}

#endif


#if TM_HANDLE_MAIN

void (*tm_key_callbacks[128])() = {0};
void (*tm_resize_callback)(int width, int height) = 0;
void (*tm_any_char_callback)(char) = 0;

void tm_bindKey(const char character, void(*function)(void)){
	if(character >= 0 && character < 128){
		tm_key_callbacks[(int)character] = function;
	}
}

/*This function is not part of the API of the library, thus not in the header*/
void callCharCallback(char character){
	if(character > 0 && character < 128){
		if(tm_key_callbacks[(int)character] != 0)
			tm_key_callbacks[(int)character]();
		if(tm_any_char_callback != 0)
			tm_any_char_callback(character);
	}
}

void tm_setResizeCallback(void(*function)(int, int)){
	tm_resize_callback = function;
}
void tm_bindToAnyKeypress(void(*function)(char)){
	tm_any_char_callback = function;
}


int main(int argc, char* argv[]){
	int tm_height, tm_width, tm_previous_width, tm_previous_height;
	tm_init();
	tm_initCall();
	tm_get_terminal_size(&tm_height, &tm_width);
	tm_previous_width = tm_width;
		tm_previous_height = tm_height;

	while(tm_run){
		tm_waitus(1000);
		tm_get_terminal_size(&tm_height, &tm_width);
		if(tm_height != tm_previous_height || tm_width != tm_previous_width){
			if(tm_resize_callback != 0)
				tm_resize_callback(tm_height, tm_width);
			tm_previous_width = tm_width;
			tm_previous_height = tm_height;
		}
		char c = tm_getch();
		if(c != 0){
			callCharCallback(c);
		}
		fflush(stdout);
	}
	tm_end();
	return 0;
}

#endif


#ifdef __cplusplus

} //end of extern C functions.

#endif
