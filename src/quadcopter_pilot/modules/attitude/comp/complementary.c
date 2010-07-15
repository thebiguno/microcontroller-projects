/*
 * A complementary filter derived from the following:
 * - http://www.rcgroups.com/forums/showpost.php?p=12082524&postcount=1286
 * - http://www.rcgroups.com/forums/showthread.php?p=11643539#post11643539
 */
vector_t int_y1;    // output of the first integrator (rad/sec)
vector_t filter;    // complementary filter output (and output of second integrator), (rad)
vector_t k;         // tuning paramater -- bandwidth of filter

void attitude_init(vector_t gyro, vector_t accel) {
    // TODO read this from EEPROM
    k.x = 1;
    k.y = 1;
    k.z = 1;

    filter.x = accel.x
    filter.y = accel.y
    filter.z = accel.z
    //account for gyro bias
    int_y1.x = -gyro.x;
    int_y1.y = -gyro.y;
    int_y1.z = -gyro.z;
}

vector_t attitude(vector_t gyro, vector_t accel, double dt) {
    vector_t int_x1; // input to the first integrator (rad/sec/sec)
    vector_t int_x2; // input to the second integrator (rad/sec)

    if (accel.z < 0.5 || filter.x > 30.0 || filter.x < -30.0) {
    	double accel_x = filter.x;
    } else {
    	double accel_x = atan(accel.x, accel.z);
    }
    if (accel.z < 0.5 || filter.y > 30.0 || filter.y < -30.0) {
    	double accel_x = filter.y;
    } else {
    	double accel_x = atan(accel.y, accel.z);
    }
    
    filter.x = _attitude(gyro.x, accel_x, &int_y1.x, &filter.x, k.x);
    filter.y = _attitude(gyro.y, accel_y, &int_y1.y, &filter.y, k.y);
    filter.z = _attitude(gyro.z, filter.z, &int_y1.z, &filter.z, k.z);
    
    return filter;
}

double _attitude(double gyro, double accel, double *_int_y1, double *_filter double k) {
    double int_x1 = (accel - *_filter) * k *  k;
    *_int_y1 += int_x1 * dt;
    double int_x2 = *_int_y1 + (accel - *_filter) * 2 *  k + gyro;
    *_filter = (int_x2 * dt) + *_filter;
    return filter;
}