vector_t angle;
vector_t bias;
vector_t p00;
vector_t p01;
vector_t p10;
vector_t p11;

vector_t q_gyro;
vector_t q_angle;
vector_t r_angle;

void attitude_init(vector_t gyro, vector_t accel) {
    // TODO read this from EEPROM
    q_angle.x = 0.001;
    q_angle.y = 0.001;
    q_angle.z = 0.001;
    q_gyro.x = 0.003;
    q_gyro.y = 0.003;
    q_gyro.z = 0.003;
    r_angle.x = 0.03;
    r_angle.y = 0.03;
    r_angle.z = 0.03;
}

vector_t attitude(vector_t gyro, vector_t accel, double dt) {
    _attitude(gyro.x, accel.x, &angle.x, &bias.x, &p00.x, &p01.x, &p10.x, &p11.x, q_gyro.x, q_angle.x, r_angle.x, dt);
    _attitude(gyro.y, accel.y, &angle.y, &bias.y, &p00.y, &p01.y, &p10.y, &p11.y, q_gyro.y, q_angle.y, r_angle.y, dt);
    _attitude(gyro.z, accel.z, &angle.z, &bias.z, &p00.z, &p01.z, &p10.z, &p11.z, q_gyro.z, q_angle.z, r_angle.z, dt);
    return angle;
}

void _attitude (double gyro, double accel, double *angle, double *bias, double *p00, double *p01, double *p10, double *p11, double q_gyro, double q_angle, double r_angle, double dt) {
    *angle += dt * (gyro - *bias);
    *p00 +=  - dt * (*p10 + *p01) + q_angle * dt;
    *p01 +=  - dt * P_11;
    *p10 +=  - dt * P_11;
    *p11 +=  + q_gyro * dt;

    double y = accel - *angle;
    double s = *p00 + r_angle;
    double k0 = *p00 / s;
    double k1 = *p10 / s;

    *angle +=  k0 * y;
    x_bias  +=  k1 * y;
    *p00 -= k0 * *p00;
    *p01 -= k0 * *p01;
    *p10 -= k1 * *p00;
    *p11 -= k1 * *p01;
}