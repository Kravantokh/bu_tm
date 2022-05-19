#include <terminal_manager.h>
#include <terminal_manager_low_level.h>
#include "benutils/unicode.h"
#include <stdio.h>
#include <locale.h>

#ifdef __linux__
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <termios.h>
	#include <sys/time.h>
	#include <sys/types.h>
#elif _WIN32
	#include <windows.h>
#else
	#error Unsupported platform.
#endif

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
/* Global variable initializations */


#if TM_HANDLE_MAIN
/* A pointer array for all the possible pressable ASCII keys.
This will be used to call the callbacks for each key. */
void (*tm_key_callbacks[128])();
void (*tm_resize_callback)(int, int);
void (*tm_any_char_callback)(char);
#endif

int tm_run = 1;

/* ** Struct manager functions ** */

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
	color.channels = (tm_rgb_color){r, g, b, a};
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

void tm_print_colored_char(tm_colored_char c){
	if(c.bg.channels.alpha > 0)
		tm_reset_color();
	else
		tm_set_bg(c.bg);
	tm_set_fg(c.fg);
		printf("%c", c.ch);
}

void tm_print_colored_uchar(tm_colored_uchar c){
	if(c.bg.channels.alpha > 0)
		tm_reset_color();
	else
		tm_set_bg(c.bg);
	tm_set_fg(c.fg);
	print_uchar(c.ch);
}
/* Functions */

/* C++ friendly library */
#ifdef __cplusplus
/*Character maps*/

extern "C" {
#endif

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
		newt.c_lflag &= ~(ICANON);
		tcsetattr( STDIN_FILENO, TCSANOW, &newt);
		newt.c_lflag &= ~(ICANON | ECHO);
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &tm_winsize);
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr( STDIN_FILENO, TCSANOW, &newt);
		setlocale(LC_ALL, "en_US");
	#endif
	/* Set locale in order for unicode characters .*/
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

void tm_getTerminalSize(int* rows, int* columns){
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
		usleep(t);
	#endif
}

/*Set rgb color for true color terminals*/
void tm_store_rgbFG(char* s, int red, int green, int blue){
	sprintf(s, "\x1b[38;2;%d;%d;%dm", red, green, blue);
}

void tm_store_rgbBG(char* s, int red, int green, int blue){
	sprintf(s, "\x1b[48;2;%d;%d;%dm", red, green, blue);
}

void tm_reset_color(){
	printf("\x1B[0m");
}

void tm_set_fg(tm_color color){
	printf("\x1b[38;2;%u;%u;%um", color.channels.red, color.channels.green, color.channels.blue);
}

void tm_set_bg(tm_color color){
	printf("\x1b[48;2;%u;%u;%um", color.channels.red, color.channels.green, color.channels.blue);
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
	char c = 0;
	FD_ISSET(0, &rfds);
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 5000;
	select(1, &rfds, NULL, NULL, &tv);
	if(FD_ISSET(0, &rfds))
		c = (char)getchar();
	return c;
	#endif
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
	tm_getTerminalSize(&tm_height, &tm_width);
	tm_previous_width = tm_width;
	tm_previous_height = tm_height;

	while(tm_run){
		tm_waitus(100);
		tm_getTerminalSize(&tm_height, &tm_width);
		if(tm_height != tm_previous_height || tm_width != tm_previous_width){
			if(tm_resize_callback != 0)
				tm_resize_callback(tm_height, tm_width);
			tm_previous_width = tm_width;
			tm_previous_height = tm_height;
		}
		char c = tm_getch();
		callCharCallback(c);

	}
	tm_end();
	return 0;
}

#endif


#ifdef __cplusplus
//Here I can finally use c++ comments, because it will not be visible for the c compiler and thus compatibility all the way back to c89 will be kept.

} //end of extern C functions.

#endif

/* copy-paste template for cross-platform functions

#ifdef _WIN32

#elif __linux__

#endif
*/
