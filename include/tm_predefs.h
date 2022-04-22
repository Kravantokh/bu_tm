#ifndef __BU_TERMINAL_MANAGER_CHAR_DEFS__
#define __BU_TERMINAL_MANAGER_CHAR_DEFS__

/* Extended ascii charcters */

#if PREDEFINE_CHARACTERS
/* Hatches */
#define HATCH_1 176
#define HATCH_2 177
#define HATCH_3 178

/* Single line table */
#define ST_ULC 218
#define ST_URC 191
#define ST_LLC 192
#define ST_LRC 217
#define ST_VER 179
#define ST_HOR 196
#define ST_RBF 195
#define ST_LBF 180
#define ST_UBF 193
#define ST_DBF 194
#define ST_CRS 197

/* Double line table */
#define DT_ULC 201
#define DT_URC 187
#define DT_LLC 200
#define DT_LRC 188
#define DT_VER 186
#define DT_HOR 205
#define DT_RBF 204
#define DT_LBF 185
#define DT_UBF 202
#define DT_DBF 203
#define DT_CRS 206

/* Filled characters */
#define UPHALF 223
#define LOHALF 220
#define LEHALF 221
#define RIHALF 222
#define FULLCH 254

/* Math characters */
#define ALPHA 224
#define BETA 225
#define GAMMA 226
#define PICH 227
#define SUMCH 228
#define SIGMA 229
#define MU 230
#define TAU 231
#define CPHI 232
#define OMEGA 234
#define DELTA 235
#define INFTY 236
#define PHI 237
#define EPSCH 238
#define CNGR 240
#define PM 241
#define GEQ 242
#define LEQ 243
#define UPINT 244
#define LOINT 245
#define DIV 246
#define AEQ 247
#define DEG 248
#define ROOT 251

#endif

#if PREDEFINE_COLORNAMES

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
