void esc_init();

/*
 * send the motor speed percentages (0 .. 1) to the speed controllers
 */
void esc_set(uint8_t armed, double speed[]);

