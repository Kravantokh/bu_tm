# Benutils Terminal Manager documentation #
## A few words ##
This library currently supports linux and windows and both C and C++.
If you compile it with C you can use the functions as they are, but if you compile it with C++ all the functions of this library are automatically put into the 'bu::tm' nested namespace. You may consider using the *using* keyword for convenience's sake in the case of frequently used functions.

The library consists of the following files:
* **terminal_manager.c** - library source file, link it.
* **terminal_manager.h** - library header file, include it.
* *tm_predefs.h* - auxiliary header file. Make sure you have it in your include directory.
* *tm_main.h* - auxiliary header file. Make sure you have it in your include directory.

## Library usage ##
For library settings see [Library settings](#Library settings).

By default the library defines the main function. The header describes an *extern* __*initCall*__() function which you may use to inject code into the beginning of the main function by defining it in your main file.

ATTENTION! You must either define this function or set __HANDLE_MAIN__ to 0 otherwise you will get linking errors.

The __*initCall*__() method can be used for setting all the callbacks. After that you should only use the callbacks to create all the functionality of your application. The _tm_run_ global integer variable can be used to quit the program. If you set it to 0 all character and resize polling will stop and the program will return 0.

## Functions ##
### Initialization ###
* *void*  **_initTerminalManager_** () - Performs some basic initialization tasks, such as initializing some variables and setting the command prompt to true color mode on windows. Use it if you define your own main, otherwise it should be omitted, because it is already executed by the library.

* *void* **_endTerminalManager_** () - Resets the console to its previous state and cleans up. Should be run once your program is done. It is automatically run if you do not define your own main.

### Color Management ###
* *void*  **_resetColor_** () - Resets the color of the text and background of the terminal to the default values.

#### RGB colors ####
* *void*  **_rgbFG_** (**int** *red*, **int** *green*, **int** *blue*) - Sets the printing text color to the given RGB value.
* *void*  **_rgbBG_** (**int** red, **int** green, **int** blue) - Sets the printing background color to the given RGB value.

* *void*  **_store_rgbFG_** (**char\*** *s*, **int** *red*, **int** *green*, **int** *blue*) - Stores the code required to switch the console to the desired RGB text color into the passed '*s*' string.

* *void*  **_store_rgbFG_** (**char\*** *s*, **int** *red*, **int** *green*, **int** *blue*) - Stores the code required to switch the console to the desired RGB background color into the passed '*s*' string.

#### Hex colors ####
For all the following functions the hex number (passed as a string literal) must use lowercase letters and may or may not begin with a *#*.

Attention! All the hex functions have some overhead compared to the rgb functions due to the need to convert from hex to rgb. If you need to or want to squeeze out absolute maximum performance of you computer you should consider using the rgb functions (which after conversion the hex functions call either way) in order to avoid  this negligible performance loss.

* *void*  **_store_hexFG_** (**char\*** *s*, const **char\*** *val*) - Stores the code required to switch the console to the desired HEX text color into the passed 's' string.

* *void*  **_store_hexBG_** (**char\*** *s*, const **char\*** *val*) - Stores the code required to switch the console to the desired HEX background color into the passed 's' string.

* *void*  **_hexFG_** (const **char\*** *val*) - Sets the printing text color to the given HEX value.

* *void*  **_hexBG_** (const **char\*** *val*) - Sets the printing background color to the given RGB value.

### Other utilities ###
* *void*  **_getTerminalSize_** (**int\*** *rows*, **int\*** *columns*) - Stores the current size of the terminal/command prompt window in number of characters into the given variables.

* *void*  **_clearTerminal_** () - Cleares the terminal's contents.

* *void* **_moveTerminalCursorTo_**(**int** *row*, **int** *column*) - Moves the cursor to the given coordinates with the help of an escape sequence.

* *void*  **_wait_** (**int** t) - Waits for the amount of time specified in second.

* *char* **_setResizeCallback_**(**void**(**\***_function_)(_int_, _int_)) - Set a callback function of type _void_ with two _int_ arguments arguments for when the key *character* is pressed. When the callback is called the number of rows and columns will be passed as arguments, in this order.
* *void **_setResizeCallback_**(**void**(**\***_function_)(*int*, *int*)) - Set a callback function of type _void_ with zero arguments for when the terminal is resized.
* *char* **_tm_getch_**() A.K.A *char* **_getch_**() - In C only the first one will work. It has *tm_* as a begininning in order to avoid name clashes. In C++ **_getch_** gets defined in the library's namespace so no problems there with name clashes. It can be used to read a character from the terminal without the need for the _enter_ key to be pressed.



## In-memory character maps ##


## Prepocessor Definitions ##

### Library settings ###
* **HANDLE_MAIN** - Should the library handle the main function? Set to yes by default. If you wish to make your own main function you should _#define_ this to 0 before including the library header. The main handles keypress callbacks and resize callbacks. If you do not use it you have to implement those on your own.
* **USE_IN_MEMORY_CHARACTER_MAPS** - Certain character maps in the form of character arrays can be loaded into memory for ease of use.
Define this preprocessor directive to 0 if you do not want to use them before including the header. The default value is 1 and no _#define_ statement is needed in order to use them. Here is a list of all the in-memory character maps created:

 * Hatches - *char* **hatch_map**[3], contents: {░, ▒, ▓}
 * Combined table - *char* **table_map**[22], contents: {┌, ┐, └, ┘, │, ─, ├, ┤, ┴, ┬, ┼, ╔, ╗, ╚, ╝, ║, ═, ╠, ╣, ╩, ╦, ╬}
* **PREDEFINE_CHARACTERS** - Defines macro names for some extended ASCII characters (see [Symbol definitions](#char_defs)). If you want to disable these macros set this to 0 before including the *terminal_manager.h*. Otherwise they will be defined automatically.

* **PREDEFINE_CHARACTERS** - Defines macro names for some change color strings (see [Symbol definitions](#color_defs)). If you want to disable these macros set this to 0 before including the *terminal_manager.h*. Otherwise they will be defined automatically.

###Symbol definitions###
<a name="char_defs">
Certain useful ascii character numbers now have names:
* Hatches
 * **HATCH_1** - Low density hatch    (░)
 * **HATCH_2** - Medium density hatch (▒)
 * **HATCH_3** - High density hatch   (▓)

* Single line table
 * **ST_ULC** - Single line table upper left corner     (┌)
 * **ST_URC** - Single line table upper right corner    (┐)
 * **ST_LLC** - Single line table lower left corner     (└)
 * **ST_LRC** - Single line table lower right corner    (┘)
 * **ST_VER** - Single line table vertical bar          (│)
 * **ST_HOR** - Single line table horizontal bar        (─)
 * **ST_RBF** - Single line table right bifurcation     (├)
 * **ST_LBF** - Single line table left bifurcation      (┤)
 * **ST_UBF** - Single line table upwards bifurcation   (┴)
 * **ST_DBF** - Single line table downwards bifurcation (┬)
 * **ST_CRS** - Single line table middle cross          (┼)

* Double line table
 * **DT_ULC** - Double line table upper left corner     (╔)
 * **DT_URC** - Double line table upper right corner    (╗)
 * **DT_LLC** - Double line table lower left corner     (╚)
 * **DT_LRC** - Double line table lower right corner    (╝)
 * **DT_VER** - Double line table vertical bar          (║)
 * **DT_HOR** - Double line table horizontal bar        (═)
 * **DT_RBF** - Double line table right bifurcation     (╠)
 * **DT_LBF** - Double line table left bifurcation      (╣)
 * **DT_RBF** - Double line table upwards bifurcation   (╩)
 * **DT_RBF** - Double line table downwards bifurcation (╦)
 * **DT_CRS** - Double line table middle cross          (╬)

* Filled blocks
 * **UPHALF** - Upper half   (▀)
 * **LOHALF** - Lower half   (▄)
 * **LEHALF** - Left half    (▌)
 * **RIHALF** - Right half   (▐)
 * **FULLCH** - Filled block (■)

* Math characters
 * **ALPHA** - α
 * **BETA**  - ß
 * **GAMMA** - Γ
 * **PICH**  - π
 * **SUMCH** - Σ
 * **SIGMA** - σ
 * **MU**    - µ
 * **TAU**   - τ
 * **CPHI**  - Φ
 * **OMEGA** - Ω
 * **DELTA** - δ
 * **INFTY** - ∞
 * **PHI**   - φ
 * **EPSCH** - ε
 * **CNGR**  - ≡
 * **PM**    - ±
 * **GEQ**   - ≥
 * **LEQ**   - ≤
 * **UPINT** - ⌠
 * **LOINT** - ⌡
 * **DIV**   - ÷
 * **AEQ**   - ≈
 * **DEG**   - °
 * **ROOT**  - √


All the ANSI colors have a keyword defined for them (you can just directly write these to the output any time to change to the given color):
<a name="color_defs">
* **FG_BLK** - black text
* **FG_RED** - red text
* **FG_GRE** - green text
* **FG_YEL** - yellow text
* **FG_BLU** - blue text
* **FG_MAG** - magenta text
* **FG_CYN** - cyan text
* **FG_WHT** - white text


* **FG_BBLK** - light black text
* **FG_BRED** - light red text
* **FG_BGRE** - light green text
* **FG_BYEL** - light yellow text
* **FG_BBLU** - light blue text
* **FG_BMAG** - light magenta text
* **FG_BCYN** - light cyan text
* **FG_BWHT** - white text


* **FG_BLK** - black background
* **FG_RED** - red background
* **FG_GRE** - green background
* **FG_YEL** - yellow background
* **FG_BLU** - blue background
* **FG_MAG** - magenta background
* **FG_CYN** - cyan background
* **FG_WHT** - white background


* **BG_BBLK** - light black background
* **BG_BRED** - light red background
* **BG_BGRE** - light green background
* **BG_BYEL** - light yellow background
* **BG_BBLU** - light blue background
* **BG_BMAG** - light magenta background
* **BG_BCYN** - light cyan background
* **BG_BWHT** - white background


* **RST** - reset the text and background to default
