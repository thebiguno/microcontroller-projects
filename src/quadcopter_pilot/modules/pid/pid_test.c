#include "pid.h"
#include <stdio.h>
#include <math.h>

#define SP -1.04719
#define PV 1.04719

int main(){

    init_pid();

    vector_t sp, pv, mv;

    sp.x = SP;
    pv.x = PV;

    int i = 0;
    while (1) {
        mv = pid_mv(sp, pv);
        pv.x += mv.x; // assume that the adjustment perfectly affects the measured value
        printf("%i: mv=%g, pv=%g\n", i, mv.x, pv.x);
        
        if (i > 100 || (pv.x > SP - 0.01 && pv.x < SP + 0.01)) break;
        i++;
    }

}
