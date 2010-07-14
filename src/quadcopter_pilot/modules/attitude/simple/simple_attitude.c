// TODO read timeConstant from EEPROM
// TODO try to undestand what these filter terms are for
#define timeConstant = 0;
#define filterTerm0 = timeConstant / (timeConstant + 0.010)//10ms = ESC update rate
#define filterTerm1 1 - filterTerm0

#define MAGIG = 57.2957795; // what is this constant??

struct vector previous_attitude = {0.0, 0.0, 0.0};

struct vector attitude(struct vector gyro, struct vector accel, double dt) {
    
    struct vector result;
    result.x = (filterTerm0 * (previous_attitude.x + (gyro.x * dt)) + filterTerm1 * (accel.x)) * MAGIC;
    result.y = (filterTerm0 * (previous_attitude.y + (gyro.y * dt)) + filterTerm1 * (accel.y)) * MAGIC;
    result.z = (filterTerm0 * (previous_attitude.x + (gyro.x * dt)) + filterTerm1 * (accel.z)) * MAGIC;
    return result;
}
