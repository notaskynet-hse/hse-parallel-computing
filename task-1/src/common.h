#ifndef COMMON_H
#define COMMON_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BLOCK 4096
#define MAX_RUNS 64

typedef float (*blockfn)(double t0, float h);

static inline void phase(double t0, float *s, float *c)
{
    *s = (float)sin(t0);
    *c = (float)cos(t0);
}

static double now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static int cmp(const void *a, const void *b)
{
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}

static double eval(blockfn f, double x, long n)
{
    double h = x / n, acc = 0;
    for (long b = 0; b < n / BLOCK; b++)
        acc += f(b * BLOCK * h, (float)h);
    return 2.0 * (acc * h + cos(x) / x + sin(x) / (x * x));
}

static void run(const char *name, blockfn f, int argc, char **argv)
{
    double x = argc > 1 ? atof(argv[1]) : 1e4;
    long n = argc > 2 ? atol(argv[2]) : 1L << 27;
    int runs = argc > 3 ? atoi(argv[3]) : 5;
    if (runs < 1 || runs > MAX_RUNS)
        runs = 5;
    n -= n % BLOCK;
    if (n < BLOCK || BLOCK * (x / n) > 0.5) {
        fprintf(stderr, "bad x/n\n");
        exit(1);
    }

    double t[MAX_RUNS], pi = 0;
    for (int r = 0; r < runs; r++) {
        double start = now();
        pi = eval(f, x, n);
        t[r] = now() - start;
    }
    qsort(t, runs, sizeof t[0], cmp);
    printf("%s,%.6f,%.9f,%.3e\n", name, t[runs / 2], pi, fabs(pi - M_PI));
}

#endif
