/* Tamás Benő 4/21/2022 */
#define HANDLE_MAIN 0
#include <terminal_manager.h>
#include <stdio.h>


int main()
{
	/* Always run this before starting to use the library */
	bu::tm::initTerminalManager();

	/* Setting text colors (16 ANSI colors) */
	printf("%sred%s\n", FG_RED, RST);
	printf("%sblack%s\n", FG_BLK, RST);
	printf("%sgreen%s\n", FG_GRE, RST);
	printf("%syellow%s\n", FG_YEL, RST);
	printf("%sblue%s\n", FG_BLU, RST);
	printf("%smagenta%s\n", FG_MAG, RST);
	printf("%scyan%s\n", FG_CYN, RST);
	printf("%swhite%s\n", FG_WHT, RST);
	printf("%sdefault%s\n", RST, RST);
	printf("%sbright red%s\n", FG_BRED, RST);
	printf("%sbright black%s\n", FG_BBLK, RST);
	printf("%sbright green%s\n", FG_BGRE, RST);
	printf("%sbright yellow%s\n", FG_BYEL, RST);
	printf("%sbright blue%s\n", FG_BBLU, RST);
	printf("%sbright magenta%s\n", FG_BMAG, RST);
	printf("%sbright cyan%s\n", FG_BCYN, RST);
	printf("%sbright white%s\n", FG_BWHT, RST);
	printf("%sdefault\n", RST);

	/* Setting background colors (16 ANSI colors) */
	printf("%sred%s\n", BG_RED, RST);
	printf("%sblack%s\n", BG_BLK, RST);
	printf("%sgreen%s\n", BG_GRE, RST);
	printf("%syellow%s\n", BG_YEL, RST);
	printf("%sblue%s\n", BG_BLU, RST);
	printf("%smagenta%s\n", BG_MAG, RST);
	printf("%scyan%s\n", BG_CYN, RST);
	printf("%swhite%s\n", BG_WHT, RST);
	printf("%sdefault%s\n", RST, RST);
	printf("%sbright red%s\n", BG_BRED, RST);
	printf("%sbright black%s\n", BG_BBLK, RST);
	printf("%sbright green%s\n", BG_BGRE, RST);
	printf("%sbright yellow%s\n", BG_BYEL, RST);
	printf("%sbright blue%s\n", BG_BBLU, RST);
	printf("%sbright magenta%s\n", BG_BMAG, RST);
	printf("%sbright cyan%s\n", BG_BCYN, RST);
	printf("%sbright white%s\n", BG_BWHT, RST);
	printf("%sdefault\n", RST);

	/* Setting truecolor RGB colors with the help of intermediary variables.*/
	char a[22];
	/* Text color */
	bu::tm::store_rgbFG(a, 3, 252, 186);
	char b[22];
	/* Background color */
	bu::tm::store_rgbBG(b, 95, 125, 0);
	/* Second text color */
	char c[22];
	bu::tm::store_hexFG(c,"#329da8");
	printf("%sTRUE%s%s%sCOLOR%s", a, RST, b, c, RST);
	printf("\n");

	/* Setting truecolor RGB colors without intermediary variables.*/
	/* Foreground color */
	bu::tm::rgbFG( 3, 252, 186);
	printf("TRUE");
	bu::tm::resetColor();
	/* Background color */
	bu::tm::hexBG("329da8");
	bu::tm::rgbFG(3, 252, 7);
	printf("COLOR");
	bu::tm::resetColor();
	printf("\n");

	int i;
	for(i = 0; i<3; ++i)
		printf("%c, ", bu::tm::hatch_map[i]);
	printf("\n");
	for(i = 0; i < 22; ++i)
		printf("%c, ", bu::tm::table_map[i]);

	return 0;
}
