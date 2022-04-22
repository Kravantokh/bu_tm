#include "terminal_manager.h"
#include <stdio.h>
/*I know it is preferred not to use a full namespace,
but in this case I think it's ok.
*/
using namespace bu::tm;

void quit(){
	tm_run = 0;
}

/* Key map for exiting */
void initCall(){
	printf("Started.");
	setCharCallback('q', &quit);
}
