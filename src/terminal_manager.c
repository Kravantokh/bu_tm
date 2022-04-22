/* Tamás Benő 4/22/2022 */
#include <terminal_manager.h>
#include <stdio.h>

#ifdef __linux__
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <termios.h>
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
#endif
/* Global variable initializations */
#if USE_IN_MEMORY_CHARACTER_MAPS

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

int tm_run = 1;

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

void initTerminalManager(){
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

	#elif __linux__
		tcgetattr( STDIN_FILENO, &oldt);
		newt = oldt;	
		newt.c_lflag &= ~(ICANON);          
		tcsetattr( STDIN_FILENO, TCSANOW, &newt);
		newt.c_lflag &= ~(ICANON | ECHO);
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &tm_winsize);
	#endif
}
void endTerminalManager(){
	#ifdef _WIN32
	SetConsoleMode(tm_outputHandle, oldMode);
	#elif __linux__
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
	#endif
}

void getTerminalSize(int* rows, int* columns){
	#ifdef _WIN32
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &tm_csbi);
		*rows = tm_csbi.srWindow.Bottom - tm_csbi.srWindow.Top + 1;
		*columns = tm_csbi.srWindow.Right - tm_csbi.srWindow.Left + 1;
	#elif __linux__
		*rows = tm_winsize.ws_row;
		*columns = tm_winsize.ws_col;
	#endif
}

void moveTerminalCursorTo(int row, int column){
	printf("\033[%d;%dH", row, column);
}
void clearTerminal(){
	#ifdef _WIN32
		system("cls");
	#elif __linux__
		system("clear");
	#endif
}

void wait(unsigned int t){
	#ifdef _WIN32
		Sleep(1000 * t); /*from seconds to milliseconds*/
	#elif __linux__
		sleep(t);
	#endif
}

void waitus(unsigned int t){
	#ifdef _WIN32
	/* Apparently windows has no uslepp function, so it has to be defined*/
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
void store_rgbFG(char* s, int red, int green, int blue){
	sprintf(s, "\x1b[38;2;%d;%d;%dm", red, green, blue);
}

void store_rgbBG(char* s, int red, int green, int blue){
	sprintf(s, "\x1b[48;2;%d;%d;%dm", red, green, blue);
}

void resetColor(){
	printf(RST);
}

void rgbFG(int red, int green, int blue){
	printf("\x1b[38;2;%d;%d;%dm", red, green, blue);
}

void rgbBG(int red, int green, int blue){
	printf("\x1b[48;2;%d;%d;%dm", red, green, blue);
}

/* Get text color string into the given string. It should be at least 22 characters long. */
void store_hexFG(char* s, const char* val){
	int r, g, b;

	if(*val == '#')
		sscanf(val, "#%2x%2x%2x", &r, &g, &b);
	else
		sscanf(val, "%2x%2x%2x", &r, &g, &b);

	sprintf(s, "\x1b[38;2;%d;%d;%dm", r, g, b);
}
/* Get background color string into the given string. It should be at least 22 characters long. */
void store_hexBG(char* s, const char* val){
	int r, g, b;

	if(*val == '#')
		sscanf(val, "#%2x%2x%2x", &r, &g, &b);
	else
		sscanf(val, "%2x%2x%2x", &r, &g, &b);

	sprintf(s, "\x1b[48;2;%d;%d;%dm", r, g, b);
}

/* Write to the console the requiered string to switch text color to the given HEX color */
void hexFG(const char* val){
	int r, g, b;
	if(*val == '#')
		sscanf(val, "#%2x%2x%2x", &r, &g, &b);
	else
		sscanf(val, "%2x%2x%2x", &r, &g, &b);
	rgbFG(r, g, b);

}
/* Write to the console the requiered string to switch background color to the given HEX color */
void hexBG(const char* val){
	int r, g, b;
	if(*val == '#')
		sscanf(val, "#%2x%2x%2x", &r, &g, &b);
	else
		sscanf(val, "%2x%2x%2x", &r, &g, &b);
	rgbBG(r, g, b);
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
		char c = getchar();
		return c;
	#endif
}


#ifdef __cplusplus
char getch(){
	return tm_getch();
}

#endif


#if HANDLE_MAIN

void (*key_callbacks[128])() = {0};
void(*resize_callback)(int width, int height) = 0;

void setCharCallback(const char character, void(*function)(void)){
	if(character >= 0 && character < 128){
		key_callbacks[(int)character] = function;
	}
}

void callCharCallback(char character){
	if(character >= 0 && character < 128){
		if(key_callbacks[(int)character] != 0)
			key_callbacks[(int)character]();
	}
}

void setResizeCallback(void(*function)(int, int)){
	resize_callback = function;
}


int main(int argc, char* argv[]){
	int tm_height, tm_width, tm_previous_width, tm_previous_height;
	initTerminalManager();
	initCall();
	getTerminalSize(&tm_height, &tm_width);
	tm_previous_width = tm_height;
	tm_previous_height = tm_width;

	while(tm_run){
		waitus(1000);
		getTerminalSize(&tm_height, &tm_width);
		if(tm_height != tm_previous_width || tm_width != tm_previous_height){
			if(resize_callback != 0)
				resize_callback(tm_height, tm_width);
			tm_previous_width = tm_height;
			tm_previous_height = tm_width;
		}
		char c = tm_getch();
		callCharCallback(c);

	}
	endTerminalManager();
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
