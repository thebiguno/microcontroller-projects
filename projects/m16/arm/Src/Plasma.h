#ifndef Plasma_H
#define Plasma_H

#include "Module.h"
#include <stdint.h>

namespace digitalcave {
    class Plasma : public Module {
    private:
        uint8_t frame;

        uint8_t width;
        uint8_t height;
        const float k = 10.0;
        float v;
        float xx;
        float yy;
        float cx;
        float cy;
        uint8_t r;
        uint8_t g;
        uint8_t b;
        float time;
        uint8_t color;
    public:
        Plasma(uint8_t width, uint8_t height);
        ~Plasma();
        void paint();
    };
}

#endif
