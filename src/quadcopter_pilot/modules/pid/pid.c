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

vector_t mv(vector_t sp, vector_t pv) {
    vector_t mv;

    e = sp.x - pv.x;
    mv.x = (kp.x * e.x) + (ki.x * ei.x) + (kd.x * (pv.x - ed.x));

    ei.x += e;
    ed.x = e;

    e = sp.y - pv.y;
    mv.y = (kp.y * e.y) + (ki.y * ei.y) + (kd.y * (pv.y - ed.y));

    ei.y += e;
    ed.y = e;

    e = sp.z - pv.z;
    mv.z = (kp.z * e.z) + (ki.z * ei.z) + (kd.z * (pv.z - ed.z));

    ei.z += e;
    ed.z = e;

    return mv;
}