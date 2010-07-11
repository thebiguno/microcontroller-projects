void i2c_init(volatile uint8_t *i2c_ddr, volatile uint8_t *i2c_pin, volatile uint8_t *i2c_port, uint8_t i2c_clock, uint8_t i2c_data);
void i2c_start();
void i2c_stop();
uint8_t i2c_write(uint8_t c);
uint8_t i2c_read(uint8_t ack);