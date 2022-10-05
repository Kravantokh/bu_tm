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
void (*tm_resize_callback)(uint16_t, uint16_t);
void (*tm_any_char_callback)(char);
#endif


bool tm_run = 1;
tm_composable* tm_root;

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

/**************** Element composition and creation related functions ****************/

tm_composable* tm_compose(tm_composable* e1, tm_composable* e2){
	e1->inner = e2;
	return e1;
}

void tm_set_root(tm_composable* new_root){
	tm_root = new_root;
}

/**************** Predefined element function ****************/



#define TM_DEF_VDIV(A)\
bool tm_vdiv_##A(tm_composable* s){\
tm_vdiv* self = (tm_vdiv*)s;

TM_DEF_VDIV(focus_up)
	switch(self->focused_element){
		case 0:
			break;
		case 1:
			if(self->inner->focus_up != NULL){
				if( self->inner->focus_up(self->inner) )
					return true;
			}
			break;
		case 2:
			if(self->composable.inner->focus_up != NULL){
				if( self->composable.inner->focus_up(self->inner) )
					return true;
			}
			break;
	}
	return false;
}

TM_DEF_VDIV(focus_down)
	switch(self->focused_element){
		case 0:
			break;
		case 1:
			if(self->inner->focus_down != NULL){
				if( self->inner->focus_down(self->inner) )
					return true;
			}
			break;
		case 2:
			if(self->composable.inner->focus_down != NULL){
				if( self->composable.inner->focus_down(self->inner) )
					return true;
			}
			break;
	}
	return false;
}

TM_DEF_VDIV(focus_left)
	printf("_FL");
	switch( self->focused_element){
		case 1:
			/* Already at the left. Can't move left. Perhaps the focused subelement can. */
			if(self->inner->focus_left != NULL ){
				/* Has the other level accepted the command? */
				if( self->inner->focus_left(self->inner) )
					return true;
			}
			break;
		case 2:
			/* The right one is focused. */
			if(self->composable.inner != NULL){
				/* Propagate the navigation command to lower levels*/
				if(self->composable.inner->focus_left != NULL ){
					/* Has the other level accepted the command? */
					if( self->composable.inner->focus_left(self->inner) )
						return true;
				}	
				/* If no one accepted the navigation we should navigate ourselves. */
				if(self->inner->focus != NULL && self->composable.inner->lose_focus != NULL){
					self->inner->lose_focus(self->inner);
					self->composable.inner->focus(self->composable.inner);
					self->focused_element = 1;
					/* Executed the navigation. Return true. */
					return true;
				}else{
					BU_ERR_FUNC("Focus or lose_focus of an element is NULL!\n");
				}
			}else{
				BU_ERR_FUNC("Tried to focus a NULL element.\n");
			}
			break;
		case 0:
			/* Nothing is focused and the focus moves from left to right (the right should be focused)*/
			printf("0");
			if( self->inner != NULL && self->inner->focus != NULL){
				self->inner->focus(self->inner);
				self->focused_element = 2;
				return true;
			}else{
				BU_ERR_FUNC("Focus or lose_focus of an element is NULL!\n");
			}
			break;
	}
	return false;
}


TM_DEF_VDIV(focus_right)
	printf("_FR");
	switch( self->focused_element){
		case 1:
			/* The left one is focused. */
			if(self->inner != NULL){
				/* Propagate the navigation command to lower levels*/
				if(self->inner->focus_right != NULL ){
					/* Has the other level accepted the command? */
					if( self->inner->focus_right(self->inner) )
						return true;
				}

				if(self->composable.inner->focus_right != NULL ){
					/* Has the other level accepted the command? */
					if( self->composable.inner->focus_right(self->composable.inner) )
						return true;
				}
				/* If no one accepted the navigation we should navigate ourselves. */
				if(self->composable.inner->focus != NULL && self->inner->lose_focus != NULL){
					self->inner->lose_focus(self->inner);
					self->composable.inner->focus(self->composable.inner);
					self->focused_element = 2;
					/* Executed the navigation. Return true. */
					return true;
				}else{
					BU_ERR_FUNC("Focus or lose_focus of an element is NULL!\n");
				}
			}else{
				BU_ERR_FUNC("Tried to focus a NULL element.\n");
			}
			break;
		case 2:
			/* Already at the right. Can't move right. Perhaps a subelement can. */
			if(self->composable.inner->focus_right != NULL ){
				/* Has the other level accepted the command? */
				if( self->composable.inner->focus_right(self->inner) )
					return true;
			}
			break;
		case 0:
			printf("0");
			/* Nothing is focused and the focus moves from left to right (the left should be focused)*/
			if( self->inner != NULL && self->inner->focus != NULL){
				self->inner->focus(self->inner);
				self->focused_element = 1;
				return true;
			}else{
				BU_ERR_FUNC("Focus or lose_focus of an element is NULL!\n");
			}
			break;
	}
	return false;
}


void tm_vdiv_focus(tm_composable* s){
	tm_vdiv* self = (tm_vdiv*)s;
	if(self->inner->lose_focus != NULL)
		self->inner->lose_focus(self->inner);
	self->focused_element = 1;	
}

void tm_vdiv_lose_focus(tm_composable* s){
	tm_vdiv* self = (tm_vdiv*)s;
	switch(self->focused_element){
		case 1:
			if(self->inner->lose_focus != NULL)
				self->inner->lose_focus(self->inner);
			break;
		case 2:
			if(self->composable.inner->lose_focus != NULL)
				self->composable.inner->lose_focus(self->composable.inner);
			break;
	}
	self->focused_element = 0;
}

void tm_vdiv_render(tm_composable* s){
	tm_vdiv* self = (tm_vdiv*)s;
	/* Propagate the rendering command. Everything should handle its own selective or non-selective refresh*/
	if(self->inner->render != NULL)
		self->inner->render(self->inner);
	
	if(self->composable.inner->render != NULL)
		self->composable.inner->render(self->composable.inner);
}

void tm_vdiv_rerender(tm_composable* s){
	tm_vdiv* self = (tm_vdiv*)s;
	if(self->inner == NULL || self->composable.inner == NULL){
		BU_ERR_FUNC("Division has NULL member.\n");
		return;
	}
	self->inner->x = self->composable.x;
	self->inner->y = self->composable.y;
	self->inner->w = self->composable.w * 0.5 + 1;
	self->inner->h = self->composable.h;

	self->composable.inner->x = self->composable.x + self->inner->w;
	self->composable.inner->y = self->composable.y;
	self->composable.inner->w = self->composable.w * 0.5;
	self->composable.inner->h = self->composable.h;

	/* Propagate the rerendering command. Everything should handle its own selective or non-selective refresh*/
	if(self->inner->rerender != NULL)
		self->inner->rerender(self->inner);
	
	if(self->composable.inner->rerender != NULL)
		self->composable.inner->rerender(self->composable.inner);
}

TM_DEF_VDIV(nav_up)
	return false;
}

TM_DEF_VDIV(nav_down)
	/* Can not move nav down in a vdiv, thus returning false.*/
	return false;
}

TM_DEF_VDIV(nav_left)
	return false;
}

TM_DEF_VDIV(nav_right)
	return false;
}

TM_DEF_VDIV(nav_sel)
	switch(self->focused_element){
		case 0:
			/* Not focused */
			return false;
		case 1:
			/* Left is focused. Allow it to take the command */
			if(self->inner->nav_sel != NULL){
				if( self->inner->nav_sel(self->inner) )
					return true;
			}
			return false;
		case 2:
			/* Right is focused. Allow it to take the command */
			if(self->composable.inner->nav_sel != NULL){
				if( self->composable.inner->nav_sel(self->inner) )
					return true;
			}
			return false;
	}

return false;
}
#undef TM_DEF_VDIV

tm_vdiv* tm_make_vdiv(tm_composable* e1, tm_composable* e2){
	tm_vdiv* vdiv = (tm_vdiv*) malloc( sizeof(tm_vdiv) );
	
	if(vdiv == NULL){
		BU_ERR_FUNC("Memory could not be allocated.");
		return NULL;
	}
	
	vdiv->focused_element = 0;
	vdiv->inner = e1;
	vdiv->composable.inner = e2;
	vdiv->composable.w = 0;
	vdiv->composable.h = 0;
	vdiv->composable.x = 0;
	vdiv->composable.y = 0;

	#define TM_GEN(A)\
	vdiv->composable. A = &tm_vdiv_##A;
	TM_GEN(focus)
	TM_GEN(lose_focus)
	TM_GEN(render)
	TM_GEN(rerender)

	TM_GEN(nav_up)
	TM_GEN(nav_down)
	TM_GEN(nav_left)
	TM_GEN(nav_right)
	TM_GEN(nav_sel)

	TM_GEN(focus_up)
	TM_GEN(focus_down)
	TM_GEN(focus_left)
	TM_GEN(focus_right)

	#undef TM_GEN
	return vdiv;
}

/******************** Debug box definitions **********************/

#define TM_DEF_DB(A)\
bool tm_debug_box_##A(tm_composable* s){\
tm_debug_box* self = (tm_debug_box*)s;

void tm_debug_box_focus(tm_composable *s) {
	tm_debug_box *self = (tm_debug_box *)s;
	tm_move_cursor(self->composable.y, self->composable.x);
	putchar('F');
}

void tm_debug_box_lose_focus(tm_composable *s) {
	tm_debug_box *self = (tm_debug_box *)s;
	tm_move_cursor(self->composable.y, self->composable.x);
	putchar('U');
}

void tm_debug_box_render(tm_composable *s) {
	tm_debug_box *self = (tm_debug_box *)s;
	int i, j = 0;
	for(i = 0; i < self->composable.h; ++i){
		tm_move_cursor(self->composable.y + i , self->composable.x);
		for(j = 0; j < self->composable.w; ++j){
			putchar(self->c);
		}
	}
}

bool tm_debug_box_false(tm_composable* s){
	return false;
}

#define tm_debug_box_rerender tm_debug_box_render

#undef TM_DEF_DB

tm_debug_box* tm_make_debug_box(char c){
	tm_debug_box* db = malloc( sizeof(tm_debug_box) );
	
	if(db == NULL){
		BU_ERR_FUNC("Memory could not be allocated.");
		return NULL;
	}
	
	db->composable.inner = NULL;
	db->composable.w = 0;
	db->composable.h = 0;
	db->composable.x = 0;
	db->composable.y = 0;
	db->c = c;

	#define TM_GEN(A)\
	db->composable. A = &tm_debug_box_##A;
	TM_GEN(focus)
	TM_GEN(lose_focus)
	TM_GEN(render)
	TM_GEN(rerender)

	#undef TM_GEN
	
	#define TM_GEN(A)\
	db->composable. A = &tm_debug_box_false;
	TM_GEN(nav_up)
	TM_GEN(nav_down)
	TM_GEN(nav_left)
	TM_GEN(nav_right)
	TM_GEN(nav_sel)
	TM_GEN(focus_up)
	TM_GEN(focus_down)
	TM_GEN(focus_left)
	TM_GEN(focus_right)

	#undef TM_GEN

	return db;
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

void tm_get_terminal_size(uint16_t* columns, uint16_t* rows){
	#ifdef _WIN32
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &tm_csbi);
		/* I don't trust the win API and my calculations, thus I added that extra cast for safety. */
		*rows = (uint16_t)(tm_csbi.srWindow.Bottom - tm_csbi.srWindow.Top + 1);
		*columns = (uint16_t)(tm_csbi.srWindow.Right - tm_csbi.srWindow.Left + 1);
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

void tm_stop(){
	tm_run = false;
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
void (*tm_resize_callback)(uint16_t width, uint16_t height) = 0;
void (*tm_any_char_callback)(char) = 0;

void tm_bindKey(const char character, void(*function)(void)){
	if(character >= 0){
		tm_key_callbacks[(int)character] = function;
	}
}

/*This function is not part of the API of the library, thus not in the header*/
void callCharCallback(char character){
	if(character > 0){
		if(tm_key_callbacks[(int)character] != 0)
			tm_key_callbacks[(int)character]();
		if(tm_any_char_callback != 0)
			tm_any_char_callback(character);
	}
}

void tm_set_resize_callback(void(*function)(uint16_t, uint16_t)){
	tm_resize_callback = function;
}
void tm_bind_any_keypress(void(*function)(char)){
	tm_any_char_callback = function;
}


int main(int argc, char* argv[]){
	uint16_t tm_height, tm_width, tm_previous_width, tm_previous_height;
	tm_init();
	tm_init_call();
	tm_get_terminal_size(&tm_width, &tm_height);
	tm_previous_width = tm_width;
	tm_previous_height = tm_height;

	while(tm_run){
		tm_waitus(1000);
		tm_get_terminal_size(&tm_width, &tm_height);
		if(tm_height != tm_previous_height || tm_width != tm_previous_width){
			if(tm_resize_callback != 0)
				tm_resize_callback(tm_width, tm_height);
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
