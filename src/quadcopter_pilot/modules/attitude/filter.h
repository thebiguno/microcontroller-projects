/*
 * given gyro_data in rad/s, accel_data in m/s/s, and delta t in seconds, return an attitude in radians (relative to the power on state)
 */
double[] filter(double[] gyro_data, double[] accel_data, double dt);