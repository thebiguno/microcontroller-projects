#define PID_X = 0
#define PID_Y = 1
#define PID_Z = 2
#define PID_PITCH = 3
#define PID_ROLL = 4
#define PID_YAW = 5

// returns an array of manipulated variables for x, y, z, pitch, roll, yaw
// sp is an array of set points (i.e. target values)
// pv is an array of process variables (i.e. measured values)
int16_t[] mv(uint16_t[] sp, uint16_t[] pv) {
