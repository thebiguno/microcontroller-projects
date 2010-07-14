// Math library required for ‘sqrt’
#include <math.h>

// System constants 
#define gyroMeasError 3.14159265358979f * (5.0f / 180.0f)                   // gyroscope measurement error in rad/s (shown as 5 deg/s)
#define beta sqrt(3.0f / 4.0f) * gyroMeasError                              // compute beta

float SEq_1 = 1.0f, SEq_2 = 0.0f, SEq_3 = 0.0f, SEq_4 = 0.0f;               // estimated orientation quaternion elements with initial conditions

struct vector attitude(struct vector gyro, struct vector accel, double dt) {
    // Local system variables
    float norm;                                                             // vector norm
    float SEqDot_omega_1, SEqDot_omega_2, SEqDot_omega_3, SEqDot_omega_4;   // quaternion derrivative from gyroscopes elements
    float f_1, f_2, f_3;                                                    // objective function elements
    float J_11or24, J_12or23, J_13or22, J_14or21, J_32, J_33;               // objective function Jacobian elements
    float SEqHatDot_1, SEqHatDot_2, SEqHatDot_3, SEqHatDot_4;               // estimated direction of the gyroscope error

    // Axulirary variables to avoid reapeated calcualtions
    float halfSEq_1 = 0.5f * SEq_1;
    float halfSEq_2 = 0.5f * SEq_2;
    float halfSEq_3 = 0.5f * SEq_3;
    float halfSEq_4 = 0.5f * SEq_4;
    float twoSEq_1 = 2.0f * SEq_1;
    float twoSEq_2 = 2.0f * SEq_2;
    float twoSEq_3 = 2.0f * SEq_3;

    // Normalise the accelerometer measurement 
    norm = sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);
    accel.x /= norm;
    accel.y /= norm;
    accel.z /= norm;

    // Compute the objective function and Jacobian
    f_1 = twoSEq_2 * SEq_4 - twoSEq_1 * SEq_3 - accel.x;
    f_2 = twoSEq_1 * SEq_2 + twoSEq_3 * SEq_4 - accel.y;
    f_3 = 1.0f - twoSEq_2 * SEq_2 - twoSEq_3 * SEq_3 - accel.z;
    J_11or24 = twoSEq_3;                                                    // J_11 negated in matrix multiplication
    J_12or23 = 2.0f * SEq_4;
    J_13or22 = twoSEq_1;                                                    // J_12 negated in matrix multiplication
    J_14or21 = twoSEq_2;
    J_32 = 2.0f * J_14or21;                                                 // negated in matrix multiplication
    J_33 = 2.0f * J_11or24;                                                 // negated in matrix multiplication

    // Compute the gradient (matrix multiplication)
    SEqHatDot_1 = J_14or21 * f_2 - J_11or24 * f_1;
    SEqHatDot_2 = J_12or23 * f_1 + J_13or22 * f_2 - J_32 * f_3;
    SEqHatDot_3 = J_12or23 * f_2 - J_33 * f_3 - J_13or22 * f_1;
    SEqHatDot_4 = J_14or21 * f_1 + J_11or24 * f_2;

    // Normalise the gradient
    norm = sqrt(SEqHatDot_1 * SEqHatDot_1 + SEqHatDot_2 * SEqHatDot_2 + SEqHatDot_3 * SEqHatDot_3 + SEqHatDot_4 * SEqHatDot_4);
    SEqHatDot_1 /= norm;
    SEqHatDot_2 /= norm;
    SEqHatDot_3 /= norm;
    SEqHatDot_4 /= norm;
    
    // Compute the quaternion derrivative measured by gyroscopes
    SEqDot_omega_1 = -halfSEq_2 * gyro.x - halfSEq_3 * gyro.y - halfSEq_4 * gyro.z;
    SEqDot_omega_2 = halfSEq_1 * gyro.x + halfSEq_3 * gyro.z - halfSEq_4 * gyro.y;
    SEqDot_omega_3 = halfSEq_1 * gyro.y - halfSEq_2 * gyro.z + halfSEq_4 * gyro.x;
    SEqDot_omega_4 = halfSEq_1 * gyro.z + halfSEq_2 * gyro.y - halfSEq_3 * gyro.x;
    
    // Compute then integrate the estimated quaternion derrivative
    SEq_1 += (SEqDot_omega_1 - (beta * SEqHatDot_1)) * deltat;
    SEq_2 += (SEqDot_omega_2 - (beta * SEqHatDot_2)) * deltat;
    SEq_3 += (SEqDot_omega_3 - (beta * SEqHatDot_3)) * deltat;
    SEq_4 += (SEqDot_omega_4 - (beta * SEqHatDot_4)) * deltat;
    
    // Normalise quaternion
    norm = sqrt(SEq_1 * SEq_1 + SEq_2 * SEq_2 + SEq_3 * SEq_3 + SEq_4 * SEq_4);
    SEq_1 /= norm;
    SEq_2 /= norm;
    SEq_3 /= norm;
    SEq_4 /= norm;
    
    // TODO convert quaternion into attitude
}