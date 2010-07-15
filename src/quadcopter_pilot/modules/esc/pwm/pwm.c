void esc_init() {
    OCR1AL = 0x40;         //Load Pulse width
    OCR1AH = 0x00;
    
    DDRB |= _BV(1) | _BV(2) | _BV(3);   // set b1,2,3 as output
    DDRD |= _BV(6);                     // set b6 as output
    
    OCR1AL = 0x40;         //Load Pulse width
    OCR1AH = 0;
    
    OCR1AL = 0x40;         //Load Pulse width
    OCR1AH = 0;

    OCR1AL = 0x40;         //Load Pulse width
    OCR1AH = 0;
    
    TCCR1A = 0x81;          //8-bit, Non-Inverted PWM
    TCCR1B = 1;             //Start PWM
}

/*
 * 
 */
void esc_set(uint8_t armed, double[] speed) {
    if (armed) {
        // TODO
        for (int i = 0; i < 4; i++) {
            _esc_set(i, scale(speed[i]));
        }
    } else {
        for (int i = 0; i < 4; i++) {
            _esc_set(i, 0);
        }
    }
}