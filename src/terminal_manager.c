#include <terminal_manager.h>
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
	DWORD tm_events;
	INPUT_RECORD tm_buffer;
	DWORD oldMode, newMode;
#elif __linux__
	struct winsize tm_winsize;
	static struct termios oldt, newt;
	struct timeval tv;
	fd_set rfds;
#endif
/* Global variable initializations */
#if TM_USE_IN_MEMORY_CHARACTER_MAPS

	#ifdef __cplusplus
	namespace bu{
	namespace tm{
	#endif

		unsigned char hatch_map[3] = {176, 177, 178};
		unsigned char table_map[22] = {
			218, 191, 192, 217, 179, 196, 195, 180, 193, 194, 197,
			201, 187, 200, 188, 186, 205, 204, 185, 202, 203, 206
		};
	#ifdef __cplusplus
	}
	}
	#endif
#endif

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

tm_color tm_create_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	tm_color color = {r, g, b, a};
	return color;
}

void tm_printChar(tm_colored_char cc){
	if(cc.bg.a > 0)
		tm_resetColor();
	else
		tm_rgbBG(cc.bg.r, cc.bg.g, cc.bg.b);
	tm_rgbFG(cc.fg.r, cc.fg.g, cc.fg.b);
		printf("%c", cc.content);
}

/* Functions */

/* C++ friendly library */
#ifdef __cplusplus
/*benutils namespace*/
namespace bu{
/* Terminal manager namespace */
namespace tm{
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
		/* Set the correct extended ASCII set*/
		system("CHCP 437");
	#elif __linux__
		tcgetattr( STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON);
		tcsetattr( STDIN_FILENO, TCSANOW, &newt);
		newt.c_lflag &= ~(ICANON | ECHO);
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &tm_winsize);
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr( STDIN_FILENO, TCSANOW, &newt);
	#endif
	/* Set locale in order for unicode characters and possibly extended ASCII to work.*/
	setlocale(LC_ALL, "");
}
void tm_end(){
	#ifdef _WIN32
	SetConsoleMode(tm_outputHandle, oldMode);
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

void tm_moveCursor(int row, int column){
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

void tm_resetColor(){
	printf("\x1B[0m");
}

void tm_rgbFG(uint8_t red, uint8_t green, uint8_t blue){
	printf("\x1b[38;2;%u;%u;%um", red, green, blue);
}

void tm_rgbBG(uint8_t red, uint8_t green, uint8_t blue){
	printf("\x1b[48;2;%u;%u;%um", red, green, blue);
}

/* Get text color string into the given string. It should be at least 22 characters long. */
void tm_store_hexFG(char* s, const char* val){
	uint8_t r, g, b;

	if(*val == '#')
		sscanf(val, "#%2x%2x%2x", &r, &g, &b);
	else
		sscanf(val, "%2x%2x%2x", &r, &g, &b);

	sprintf(s, "\x1b[38;2;%u;%u;%um", r, g, b);
}
/* Get background color string into the given string. It should be at least 22 characters long. */
void tm_store_hexBG(char* s, const char* val){
	uint8_t r, g, b;

	if(*val == '#')
		sscanf(val, "#%2x%2x%2x", &r, &g, &b);
	else
		sscanf(val, "%2x%2x%2x", &r, &g, &b);

	sprintf(s, "\x1b[48;2;%u;%u;%um", r, g, b);
}

/* Write to the console the requiered string to switch text color to the given HEX color */
void tm_hexFG(const char* val){
	uint8_t r, g, b;
	if(*val == '#')
		sscanf(val, "#%2x%2x%2x", &r, &g, &b);
	else
		sscanf(val, "%2x%2x%2x", &r, &g, &b);
	tm_rgbFG(r, g, b);

}
/* Write to the console the requiered string to switch background color to the given HEX color */
void tm_hexBG(const char* val){
	uint8_t r, g, b;
	if(*val == '#')
		sscanf(val, "#%2x%2x%2x", &r, &g, &b);
	else
		sscanf(val, "%2x%2x%2x", &r, &g, &b);
	tm_rgbBG(r, g, b);
}

char tm_getch(){
	#ifdef _WIN32
	PeekConsoleInput(tm_inputHandle, &tm_buffer, 1, &tm_events);
	if(tm_events > 0){
		/* For some reason it reads every key twice unless I do this terribleness of reading it twice myself.*/
		ReadConsoleInput(tm_inputHandle, &tm_buffer, 1, &tm_events);
		ReadConsoleInput(tm_inputHandle, &tm_buffer, 1, &tm_events);
		return tm_buffer.Event.KeyEvent.uChar.AsciiChar;
	}
	else return 0;

	#elif __linux__
	char c = 64;
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
	if(character >= 0 && character < 128){
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
		tm_waitus(1000);
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
//Here I can finally use c++ comments, cuz it will not be visible for the c compiler and thus compatibility all the way back to c89 will be kept.
//Yes I left that comment just for fun.

} //end of extern C functions.

} //namespace tm
} //namespace bu

#endif

/*
#ifdef _WIN32

#elif __linux__

#endif
*/
