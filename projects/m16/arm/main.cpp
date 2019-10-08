#include "main.h"
#include "Src/ph42121rgb16s.h"
#include "Src/App.h"
#include "Src/Matrix.h"

extern "C" {
	void run();
}

using namespace digitalcave;

void run() {
    ph42121rgb16s_init();

    App app = App();
    app.run();

    while (1) {
    }
}

extern "C" void __cxa_pure_virtual() { while (1); }
