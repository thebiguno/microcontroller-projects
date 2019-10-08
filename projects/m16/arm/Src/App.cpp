#include "App.h"
#include "main.h"

using namespace digitalcave;

Matrix matrix = Matrix();
Hsv hsv = Hsv(0,0xff,0x1f);
//Darksky darksky = Darksky(&wifi, &state);
//SerialHAL serial = SerialHAL(&huart, 2048);
//ESP8266 wifi = ESP8266(&serial);
//State state = State();

App::App() {
}

App::~App() {
}

void App::run() {
    Life life = Life(32, 64);
    while(1) {
        life.paint();

        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        HAL_Delay(100);
    }
}

    /*

    wifi.at_cipserver_tcp(1, 80);

    int mode = 0;
    while (1) {
        // blink
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

        // paint
        life.paint();

        // handle buttons

        // accept wifi requests
        auto acceptor = [](Stream* s) { accept(WebStream(s)); };
        wifi.accept(&acceptor);

        // accept serial requests (debug / bluetooth)

        // fetch weather updates
        if (darksky.stale()) {
            darksky.update(&state);
        }
    }

    */


    /*
void accept(WebStream* s) {
    if (strcmp("POST", s->method()) == 0) {
        if (strcmp("/time"), s->path() == 0) {
            JsonParser parser = JsonParser(s);
            uint8_t token;
            while ((token = parser.nextToken()) != TOKEN_EOF) {
                if (parser.nextToken() == TOKEN_NUMBER) {
                    char buf[12];
                    parser.getValue(buf);
                    uint32_t time = atol(buf);
                    state.setTime(time);
                }
            }
        }
    }
}
    */

    /*
void paint() {

    // only paint every 10 ms
    if (time - last_paint < 10) return;
    last_paint = time;

    // what gets painted depends on the time
    // a time slice is 10 seconds and every 5 minutes the cycle repeats
    uint8_t slice = (time / 1000 / 10) % 30;

    if (slice == 0 || slice == 6 || slice == 12 || slice == 18 || slice == 24) {
        // current time
    } else if (slice == 5 || slice == 11 || slice == 17 || slice == 23 || slice == 29) {
        // random image / animation between 50 and 59 seconds
    } else if (slice > 6 && slice < 11) {
        // plasma
    } else if (slice > 18 && slice < 23) {
        // game of life
    } else if (slice == 1) {
        // date
    } else if (slice == 2) {
        // current temperature / temperature icon
    } else if (slice == 3) {
        //  barometer
    } else if (slice == 13) {
        // percentage of day (metric time)
    } else if (slice == 14) {
        // temperature high / low
    } else if (slice == 25) {
        // hexadecimal time
    } else if (slice == 26) {
        // sunrise / sunset
    }

}
    */
