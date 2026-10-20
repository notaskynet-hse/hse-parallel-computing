#include "common.h"

#ifndef STAGE
#define STAGE "naive"
#endif

__attribute__((noinline)) static float block_naive(double t0, float h)
{
    float s0, c0;
    phase(t0, &s0, &c0);
    float tf = (float)t0, s = 0;
    for (int j = 0; j < BLOCK; j++) {
        float u = (j + 0.5f) * h;
        s += (s0 * cosf(u) + c0 * sinf(u)) / (tf + u);
    }
    return s;
}

int main(int argc, char **argv)
{
    run(STAGE, block_naive, argc, argv);
    return 0;
}
