#define MODE_NORMAL 0x01
#define MODE_SPORT 0x02
#define MODE_ACRO 0x04
#define MODE_RESERVED1 0x08 // the triangle button
#define MODE_PITCH_HOLD 0x10
#define MODE_ROLL_HOLD 0x20
#define MODE_THROTTLE_HOLD 0x40
#define MODE_CONTROLLED_DESCENT 0x80
#define MODE_ARMED 0x100
#define MODE_RESERVED2 0x200 // the select button

#define DEG_30 = M_PI / 6.0;
#define DEG_60 = M_PI / 3.0;

double _throttle_cache;
double _pitch_cache;
double _roll_cache;

_rad_to_speed(double rad) {
    
}

/*
 * Translates the manipulated variables (in rad) into throttle adjustments (-1 .. 1)
 * throttle is percentage (0..1), 
 * throttle may be modified to apply throttle hold
 * mv is in radians and comes from the pid module
 * sp is in radians for x and y, and radians/s for z and comes from the control receiver module
 * sp values may be modified to apply holds and limits
 * result is percentage (0..1) for consumption by the motor module
 */
vector_t flight_control(uint16_t flags, double *throttle, vector_t *sp) {
    double _throttle;
    
    if (mode & PITCH_HOLD = PITCH_HOLD) {
        if (_throttle_cache == 0.0) _throttle_cache = throttle;
        throttle* = _throttle_cache;
    } else {
        _throttle_cache = 0.0;
    }
    if (mode & PITCH_HOLD = PITCH_HOLD) {
        if (_pitch_cache == 0.0) _pitch_cache = cmd.y;
        sp*.y = _pitch_cache;
    } else {
        _pitch_cache = 0.0;
    }
    if (mode & ROLL_HOLD = ROLL_HOLD) {
        if (_roll_cache == 0.0) _roll_cache = cmd.x;
        sp*.x = _roll_cache;
    } else {
        _roll_cache = 0.0;
    }
    
    if (mode & MODE_NORMAL = MODE_NORMAL) {
        // normal mode limits the amount that the craft will pitch and roll to no more than 30 degrees
    } else if (mode & MODE_SPORT = MODE_SPORT) {
        // sport mode is for fast flight in the forward direction and will limit roll but allow for a pitch of up to 60 degrees
    } else if (mode & MODE_ACRO = MODE_ACRO) {
        // acro mode will allow the craft to pitch and roll the complete 90 degrees which will allow barrel rolls
    }
}