/*
 * Sets the clock mode.
 * 0: traditional (24:59:59)
 * 1: vigesimal(19:19:19:19)
 * 1: hexadecimal (F:F:F:F)
 * 2: decimal (9:9:9:9)
 * 3: octal (8:8:8:8:8:8)
 */
void clock_mode(uint8_t mode);

/*
 * Recalculates the segment and matrix state.
 */
void clock_update(uint16_t millis);

/*
 * Sets a char array according to the time in the given mode.
 */
void clock_segments(char[] result);

/*
 * Sets a matrix array according to the time in the given mode.
 */
void clock_matrix(uint8_t[] result);