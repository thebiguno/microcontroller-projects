#include "LedTable.h"
#include "Life.h"
#include "lib/timer/timer.h"

using namespace digitalcave;

int main() {
	timer_init();
	
	Life life;
	life.start();
	
	while (1) {
		;
	}
}