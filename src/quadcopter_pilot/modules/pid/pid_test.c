#include "pid.h"
#include <stdio.h>

int main(){
	
	init_pid();

	vector_t sp, pv, mvr;

	sp.x = 0;
	sp.y = 1;
	sp.z = 0;

	pv.x = 0;
	pv.y = 2;
	pv.z = 0;

	mvr = mv(sp, pv);
	printf("%g, %g, %g\n", mvr.x, mvr.y, mvr.z);
	pv.y += mvr.y;
	
	mvr = mv(sp, pv);
	printf("%g, %g, %g\n", mvr.x, mvr.y, mvr.z);
	pv.y += mvr.y;

	mvr = mv(sp, pv);
	printf("%g, %g, %g\n", mvr.x, mvr.y, mvr.z);
	pv.y += mvr.y;

	mvr = mv(sp, pv);
	printf("%g, %g, %g\n", mvr.x, mvr.y, mvr.z);
	pv.y += mvr.y;

	mvr = mv(sp, pv);
	printf("%g, %g, %g\n", mvr.x, mvr.y, mvr.z);
	pv.y += mvr.y;

	mvr = mv(sp, pv);
	printf("%g, %g, %g\n", mvr.x, mvr.y, mvr.z);
	pv.y += mvr.y;

	mvr = mv(sp, pv);
	printf("%g, %g, %g\n", mvr.x, mvr.y, mvr.z);
	pv.y += mvr.y;
	
}
