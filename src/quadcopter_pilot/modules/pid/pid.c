float[] kp = {1,1,1};
float[] ki = {1,1,1};
float[] kd = {1,1,1};

float[] ei = {0,0,0};
float[] ed = {0,0,0};

float[] mv(float[] sp, float[] pv) {
    int16_t[] mv = {0,0,0};
    for (int i = 0; i < 3; i++) {
        e = sp[i] - pv[i];
        
        float p = kp[i] * e[i];
        float i = ki[i] * ei[i];
        float d = kd[i] * (pv - ed[i]);
        
        ei[i] += e;
        ed[i] = e;
        
        mv[i] = p + i + d;
    }
    return mv;
}