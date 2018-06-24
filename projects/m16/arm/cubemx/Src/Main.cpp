#include "LedTable.h"

using namespace digitalcave;

Matrix matrix = Matrix();

int main() {
    srandom(0);

    int mode = 0;
    while (1) {

        if (mode == 0) {
            Clock clock;
            clock.run();
        } else if (mode == 1) {
            Plasma plasma;
            plasma.run();
        } else if (mode == 2) {
            Weather weather;
            weather.run();
        } else if (mode == 3) {
            Life life;
            life.run();
        }
    }
    return 0;
}

extern "C" void __cxa_pure_virtual() { while (1); }
