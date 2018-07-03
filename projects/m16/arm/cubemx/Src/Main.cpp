#include "LedTable.h"

using namespace digitalcave;

Matrix matrix = Matrix();
SerialHAL serial = SerialHAL(&huart, 2048);
ESP8266 wifi = ESP8266(&serial);

int main() {
    srandom(0);

    int mode = 0;
    while (1) {
        _delay(10);

        // paint module(s)
        module.paint();

        // handle buttons

        // receive wifi

        // receive serial

        // fetch weather updates
        if (0) {    // every hour ?
            Darksky darksky = Darksky(&wifi, &state);
            darksky.update(&state);
        }
    }
    return 0;
}

extern "C" void __cxa_pure_virtual() { while (1); }
