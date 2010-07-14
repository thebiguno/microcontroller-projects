// scale from  (0x0000 .. 0xFFFF) to (1000 to 2000)
uint16_t scale(uint16_t x) {
    // y = mx + b
    // m = (y2 - y1) / (x2 - x1) = (2000 - 1000) / (65535 - 0)
    // b = y1 - m * x1 = (2000 - 0.0152...) * 0 = 1000
    
    float m = 0.015259021896696;
    float b = 1000;
    return (uint16_t) m * x + b;
}
