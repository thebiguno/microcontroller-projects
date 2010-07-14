#include "pid.h"

vector_t kp;
vector_t ki;
vector_t kd;

vector_t ei;
vector_t ed;

void init_pid() {
    // TODO get tuning values K from EEPROM
    kp.x = 1;
    kp.y = 1;
    kp.z = 1;
    
    ki.x = 1;
    ki.y = 1;
    ki.z = 1;
    
    kd.x = 1;
    kd.y = 1;
    kd.z = 1;
}

double _mv(double sp, double pv, double kp, double ki, double kd, double *ei, double *ed){
    double e = sp - pv;
    double mv = (kp * e) + (ki * *ei) + (kd * (pv - *ed));

    *ei += e;
    *ed = e;
    
    return mv;
}

vector_t mv(vector_t sp, vector_t pv) {
    vector_t mv;

	mv.x = _mv(sp.x, pv.x, kp.x, ki.x, kd.x, &ei.x, &ed.x);

	mv.y = _mv(sp.y, pv.y, kp.y, ki.y, kd.y, &ei.y, &ed.y);
	
	mv.z = _mv(sp.z, pv.z, kp.z, ki.z, kd.z, &ei.z, &ed.z);

    return mv;
}

