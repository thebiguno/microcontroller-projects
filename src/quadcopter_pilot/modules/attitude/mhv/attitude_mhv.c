#include "../../../main.h"

static double gain;
static double beta;
    
typedef struct quaternion {
    double w;
    double x;
    double y;
    double z;
} quaternion_t;

quaternion_t q;
    
void attitude_init(vector_t gyro, vector_t accel) {
    // TODO read these values from EEPROM
    
    // gyroscope measurement error in rad/s (shown as 5 deg/s)
    gain = M_PI * (5.0 / 180.0); 
    beta = sqrt(3.0 / 4.0) * gain;
    
    
    q.w = 1.0;
    q.x = 0.0;
    q.y = 0.0;
    q.z = 0.0;
}

vector_t attitude(vector_t gyro, vector_t accel, double dt) {
    // Local system variables
    double norm;                                                    // vector norm
    double f_1, f_2, f_3;                                           // objective function elements
    double J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33;      // objective function Jacobian elements
    quaternion_t q_dot_omega;                                       // quaternion derrivative from gyroscopes elements
    quaternion_t q_hat_dot;                                         // estimated direction of the gyroscope error

    quaternion_t half_q;
    quaternion_t two_q;
    
    // Axulirary variables to avoid reapeated calcualtions
    half_q.w = 0.5 * q.w;
    half_q.x = 0.5 * q.x;
    half_q.y = 0.5 * q.y;
    half_q.z = 0.5 * q.z;
    two_q.w = 2.0 * q.w;
    two_q.x = 2.0 * q.x;
    two_q.y = 2.0 * q.y;

    // Normalise the accelerometer measurement 
    norm = sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);
    accel.x /= norm;
    accel.y /= norm;
    accel.z /= norm;

    // Compute the objective function and Jacobian
    f_1 = two_q.x * q.z - two_q.w * q.y - accel.x;
    f_2 = two_q.w * q.x + two_q.y * q.z - accel.y;
    f_3 = 1.0 - two_q.x * q.x - two_q.y * q.y - accel.z;
    J_11or24 = two_q.y;                                                    // J_11 negated in matrix multiplication
    J_12or23 = 2.0 * q.z;
    J_13or22 = two_q.w;                                                    // J_12 negated in matrix multiplication
    J_14or21 = two_q.x;
    J_32 = 2.0 * J_14or21;                                                 // negated in matrix multiplication
    J_33 = 2.0 * J_11or24;                                                 // negated in matrix multiplication

    // Compute the gradient (matrix multiplication)
    q_hat_dot.w = J_14or21 * f_2 - J_11or24 * f_1;
    q_hat_dot.x = J_12or23 * f_1 + J_13or22 * f_2 - J_32 * f_3;
    q_hat_dot.y = J_12or23 * f_2 - J_33 * f_3 - J_13or22 * f_1;
    q_hat_dot.z = J_14or21 * f_1 + J_11or24 * f_2;

    // Normalise the gradient
    norm = sqrt(q_hat_dot.w * q_hat_dot.w + q_hat_dot.x * q_hat_dot.x + q_hat_dot.y * q_hat_dot.y + q_hat_dot.z * q_hat_dot.z);
    q_hat_dot.w /= norm;
    q_hat_dot.x /= norm;
    q_hat_dot.y /= norm;
    q_hat_dot.z /= norm;
    
    // Compute the quaternion derrivative measured by gyroscopes
    q_dot_omega.w = -half_q.x * gyro.x - half_q.y * gyro.y - half_q.z * gyro.z;
    q_dot_omega.x = half_q.w * gyro.x + half_q.y * gyro.z - half_q.z * gyro.y;
    q_dot_omega.y = half_q.w * gyro.y - half_q.x * gyro.z + half_q.z * gyro.x;
    q_dot_omega.z = half_q.w * gyro.z + half_q.x * gyro.y - half_q.y * gyro.x;
    
    // Compute then integrate the estimated quaternion derrivative
    q.w += (q_dot_omega.w - (beta * q_hat_dot.w)) * dt;
    q.x += (q_dot_omega.x - (beta * q_hat_dot.x)) * dt;
    q.y += (q_dot_omega.y - (beta * q_hat_dot.y)) * dt;
    q.z += (q_dot_omega.z - (beta * q_hat_dot.z)) * dt;
    
    // Normalise quaternion
    norm = sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
    q.w /= norm;
    q.x /= norm;
    q.y /= norm;
    q.z /= norm;
    
    // convert quaternion into attitude
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
    // TODO verify that heading, bank, and attitude are as expected
    
    vector_t result;
    double test = q.x * q.y + q.z * q.w;
    if (test > 0.499) { // singularity at north pole
        result.z = 2 * atan2(q.x, q.w);
        result.y = M_PI/2;
        result.x = 0;
        return result;
    }
    if (test < -0.499) { // singularity at south pole
        result.z = -2 * atan2(q.x, q.w);
        result.y = - M_PI/2;
        result.x = 0;
        return result;
    }
    double sqx = q.x * q.x;
    double sqy = q.y * q.y;
    double sqz = q.z * q.z;
    result.z = atan2(2 * q.y * q.w - 2 * q.x * q.z, 1 - 2 * sqy - 2 * sqz);
    result.y = asin(2 * test);
    result.x = atan2(2 * q.x * q.w - 2 * q.y * q.z, 1 - 2 * sqx - 2 * sqz);
    
    return result;
}