#include <math.h>
#include <util/delay.h>

#include "../../../main.h"
#include "../../protocol/protocol.h"
#include "../../persist/persist.h"
#include "../../util/convert.h"

typedef struct kalman {
	// x
	double alpha;
	double bias;
	// P
	double p00;
	double p01;
	double p10;
	double p11;
	// Sw
	double q_gyro;		// E(alpha^2)
	double q_angle;		// E(bias^2)
	// Sz
	double r_angle;
} kalman_t;

static kalman_t state_x;
static kalman_t state_y;

void attitude_read_tuning() {
	uint8_t data[24];
	
	uint8_t length = persist_read(PERSIST_SECTION_ATTITUDE, data, 24);
	if (length == 24) {
		state_x.q_angle = convert_bytes_to_double(data, 0);
		state_x.q_gyro = convert_bytes_to_double(data, 4);
		state_x.r_angle = convert_bytes_to_double(data, 8);

		state_y.q_angle = convert_bytes_to_double(data, 12);
		state_y.q_gyro = convert_bytes_to_double(data, 16);
		state_y.r_angle = convert_bytes_to_double(data, 20);
	} else {
		status_error(STATUS_ERR_ATTITUDE_CHECKSUM);
		_delay_ms(1000);
		status_error(0x00);
		
		state_x.q_angle = 0.001;
		state_x.q_gyro = 0.003;
		state_x.r_angle = 0.3;

		state_y.q_angle = 0.001;
		state_y.q_gyro = 0.003;
		state_y.r_angle = 0.3;
	}
}

void attitude_write_tuning() {
	uint8_t data[24];
	
	convert_double_to_bytes(state_x.q_angle, data, 0);
	convert_double_to_bytes(state_x.q_gyro, data, 4);
	convert_double_to_bytes(state_x.r_angle, data, 8);
	
	convert_double_to_bytes(state_y.q_angle, data, 12);
	convert_double_to_bytes(state_y.q_gyro, data, 16);
	convert_double_to_bytes(state_y.r_angle, data, 20);
	
	persist_write(PERSIST_SECTION_ATTITUDE, data, 24);
}

void attitude_init(vector_t gyro, vector_t accel) {
	attitude_read_tuning();
}

void _attitude (double gyro, double accel, kalman_t *state, double dt) {
	// http://tom.pycke.be/mav/71/kalman-filtering-of-imu-data
	
	// A = [1, -dt][0, 1]
	// B = [dt, 0]
	// C = [1, 0]'
	// x = [alpha bias]'
	// u = gyro measurement (rad/s)
	// y = accel measurement (m/s/s)
	
	// Update the state x of the model (x = A · x + B · u)
	// Simplifies to alpha[k]= alpha[k-1] + (_u_[k] – bias);
	state->alpha += dt * (gyro - state->bias);

	// Calculate the covariance of the prediction error
	// P = A · P · A' – K · C · P · A' + Sw
	// See the link above for more details
	state->p00 +=  - dt * (state->p10 + state->p01) + state->q_angle * dt;
	state->p01 +=  - dt * state->p11;
	state->p10 +=  - dt * state->p11;
	state->p11 +=  + state->q_gyro * dt;

	// Calculate the difference between the second value and the value predicted by our model
	// This is called the innovation
	// Inn = y – C · x
	// The C matrix is the one that extracts the ouput from the state matrix. In our case, this is (1 0)': alpha = C · x
	double inno = accel - state->alpha;

	// Calculate the covariance
	// s = C · P · C' + Sz
	// Sz is the measurement process noise covariance in radians = E(z[k] * z[k]^T); i.e. accel jitter
	double s = state->p00 + state->r_angle;
	
	// Calculate the kalman gain
	// K = A · P · C' · inv(_s_)
	double k0 = state->p00 / s;
	double k1 = state->p10 / s;

	// Correct the prediction of the state
	// x = x + K · Inn
	state->alpha +=  k0 * inno;
	state->bias +=  k1 * inno;
	
	state->p00 -= k0 * state->p00;
	state->p01 -= k0 * state->p01;
	state->p10 -= k1 * state->p00;
	state->p11 -= k1 * state->p01;
}

vector_t attitude(vector_t gyro, vector_t accel, double dt) {
	_attitude(gyro.x, accel.x, &state_x, dt);
	_attitude(gyro.y, accel.y, &state_y, dt);
	
	static vector_t result;
	result.x = state_x.alpha;
	result.y = state_y.alpha;
	return result;
}

uint8_t attitude_get_id() {
	return 'K';
}

void attitude_reset() {
	state_x.alpha = 0.0;
	state_y.alpha = 0.0;
}

void attitude_send_tuning() {
	uint8_t length = 24;
	uint8_t buf[length];
	
	convert_double_to_bytes(state_x.q_angle, buf, 0);
	convert_double_to_bytes(state_x.q_gyro, buf, 4);
	convert_double_to_bytes(state_x.r_angle, buf, 8);

	convert_double_to_bytes(state_y.q_angle, buf, 12);
	convert_double_to_bytes(state_y.q_gyro, buf, 16);
	convert_double_to_bytes(state_y.r_angle, buf, 20);

	protocol_send_message('k', buf, length);
}

void attitude_receive_tuning(uint8_t *buf) {
	state_x.q_angle = convert_bytes_to_double(buf, 0);
	state_x.q_gyro = convert_bytes_to_double(buf, 4);
	state_x.r_angle = convert_bytes_to_double(buf, 8);

	state_y.q_angle = convert_bytes_to_double(buf, 12);
	state_y.q_gyro = convert_bytes_to_double(buf, 16);
	state_y.r_angle = convert_bytes_to_double(buf, 20);
}

