double[] kp = {1,1,1};
double[] ki = {1,1,1};
double[] kd = {1,1,1};

int16_t[] ei = {0,0,0};
int16_t[] ed = {0,0,0};

uint16_t[] mv(uint16_t[] sp, uint16_t[] pv) {
    int16_t[] mv = {0,0,0};
    for (int i = 0; i < 3; i++) {
        e = sp[i] - pv[i];
        
        p = kp[i] * e[i];
        i = ki[i] * ei[i];
        d = kd[i] * (pv - ed[i]);
        
        ei[i] += e;
        ed[i] = e;
        
        mv[i] = p + i + d;
    }
    return mv;
}