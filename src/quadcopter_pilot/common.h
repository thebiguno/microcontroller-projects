#ifndef COMMON_H
#define COMMON_H

void set_data(char *data);

void send_debug_data(uint8_t *data);

typedef struct vector{
    double x;
    double y;
    double z;
} vector_t;


#endif
