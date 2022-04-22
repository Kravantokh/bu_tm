#ifndef __BU_TERMINAL_MANAGER_MAIN__
#define __BU_TERMINAL_MANAGER__MAIN__

#if HANDLE_MAIN

/* A pointer array for all the possible pressable ASCII keys.
This will be used to call the callbacks for each key. */
extern void (*key_callbacks[128])();
extern void(*resize_callback)(int, int);

extern void initCall();

extern void setCharCallback(const char character, void(*function)(void));
/*extern void setResizeCallback(void(*function)(int, int);*/

#endif

#endif
