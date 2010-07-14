#include "pid.h"

vector_t kp;
vector_t ki;
vector_t kd;

vector_t ei;
vector_t ed;

void init_pid() {
    // TODO get tuning values K from EEPROM
    kp.x = 0.7;
    kp.y = 0.7;
    kp.z = 0.0;
    
    ki.x = 0.001;
    ki.y = 0.0;
    ki.z = 0.0;
    
    kd.x = 0.001;
    kd.y = 0.0;
    kd.z = 0.0;
}

double _pid_mv(double sp, double pv, double kp, double ki, double kd, double *ei, double *ed){
    double e = sp - pv;
    double mv = (kp * e) + (ki * *ei) + (kd * (pv - *ed));

    *ei += e;
    *ed = e;
    
    return mv;
}

vector_t pid_mv(vector_t sp, vector_t pv) {
    vector_t mv;

	mv.x = _pid_mv(sp.x, pv.x, kp.x, ki.x, kd.x, &ei.x, &ed.x);
	mv.y = _pid_mv(sp.y, pv.y, kp.y, ki.y, kd.y, &ei.y, &ed.y);
	mv.z = _pid_mv(sp.z, pv.z, kp.z, ki.z, kd.z, &ei.z, &ed.z);

    return mv;
}

