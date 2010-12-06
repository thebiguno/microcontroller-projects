#include "pid.h"

vector_t _kp;
vector_t _ki;
vector_t _kd;

typedef struct err {
    double i;
    double d;
} pid_err_t;

pid_err_t _err_x;
pid_err_t _err_y;
pid_err_t _err_z;

void pid_init() {
    // TODO get tuning values K from EEPROM
    vector_t kp = {0.7,0.7,0.0};
    vector_t ki = {0.001,0.0,0.0};
    vector_t kd = {0.001,0.0,0.0};
    pid_set_params(&kp, &ki, &kd);
}

void pid_set_params(vector_t *kp, vector_t *ki, vector_t *kd) {
    _kp.x = kp->x;
    _kp.y = kp->y;
    _kp.z = kp->z;
    _ki.x = ki->x;
    _ki.y = ki->y;
    _ki.z = ki->z;
    _kd.x = kd->x;
    _kd.y = kd->y;
    _kd.z = kd->z;
}
void pid_get_params(vector_t *kp, vector_t *ki, vector_t *kd) {
    kp->x = _kp.x;
    kp->y = _kp.y;
    kp->z = _kp.z;
    ki->x = _ki.x;
    ki->y = _ki.y;
    ki->z = _ki.z;
    kd->x = _kd.x;
    kd->y = _kd.y;
    kd->z = _kd.z;
}

double _pid_mv(double sp, double pv, double kp, double ki, double kd, pid_err_t *err){
    double e = sp - pv;
    double mv = (kp * e) + (ki * err->i) + (kd * (pv - err->d));

    err->i += e;
    err->d = e;
    
    return mv;
}

vector_t pid_mv(vector_t sp, vector_t pv) {
    vector_t mv;

	mv.x = _pid_mv(sp.x, pv.x, _kp.x, _ki.x, _kd.x, &_err_x);
	mv.y = _pid_mv(sp.y, pv.y, _kp.y, _ki.y, _kd.y, &_err_y);
	mv.z = _pid_mv(sp.z, pv.z, _kp.z, _ki.z, _kd.z, &_err_z);

    return mv;
}

