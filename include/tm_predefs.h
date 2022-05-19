/** \file tm_predefs.h
* \brief A separate header for some macros.
*/
#ifndef __BU_TERMINAL_MANAGER_CHAR_DEFS__
#define __BU_TERMINAL_MANAGER_CHAR_DEFS__

#if TM_PREDEFINE_COLORNAMES

/****  Color names ****/
#define FG_BLK "\x1b[30m"
#define FG_RED "\x1B[31m"
#define FG_GRE "\x1B[32m"
#define FG_YEL "\x1B[33m"
#define FG_BLU "\x1B[34m"
#define FG_MAG "\x1B[35m"
#define FG_CYN "\x1B[36m"
#define FG_WHT "\x1B[37m"

#define FG_BBLK "\x1b[30;1m"
#define FG_BRED "\x1B[31;1m"
#define FG_BGRE "\x1B[32;1m"
#define FG_BYEL "\x1B[33;1m"
#define FG_BBLU "\x1B[34;1m"
#define FG_BMAG "\x1B[35;1m"
#define FG_BCYN "\x1B[36;1m"
#define FG_BWHT "\x1B[37;1m"

#define BG_BLK "\x1b[40m"
#define BG_RED "\x1B[41m"
#define BG_GRE "\x1B[42m"
#define BG_YEL "\x1B[43m"
#define BG_BLU "\x1B[44m"
#define BG_MAG "\x1B[45m"
#define BG_CYN "\x1B[46m"
#define BG_WHT "\x1B[47m"

#define BG_BBLK "\x1b[40;1m"
#define BG_BRED "\x1B[41;1m"
#define BG_BGRE "\x1B[42;1m"
#define BG_BYEL "\x1B[43;1m"
#define BG_BBLU "\x1B[44;1m"
#define BG_BMAG "\x1B[45;1m"
#define BG_BCYN "\x1B[46;1m"
#define BG_BWHT "\x1B[47;1m"

#define RST "\x1B[0m"

#endif

#endif
