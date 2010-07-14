#define PID_PITCH = 0
#define PID_ROLL = 1
#define PID_YAW = 2

// returns an array of manipulated variables for pitch, roll, yaw
// sp is an array of set points (i.e. target values)
// pv is an array of process variables (i.e. measured values)
int16_t[] mv(int16_t[] sp, int16_t[] pv) {
