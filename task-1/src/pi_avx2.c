#include <immintrin.h>

#include "common.h"

#define INLINE static inline __attribute__((always_inline))

INLINE __m256 term(__m256 u, __m256 s0, __m256 c0, __m256 t0)
{
    __m256 u2 = _mm256_mul_ps(u, u);

    __m256 sn = _mm256_fmadd_ps(u2, _mm256_set1_ps(1.0f / 362880), _mm256_set1_ps(-1.0f / 5040));
    sn = _mm256_fmadd_ps(u2, sn, _mm256_set1_ps(1.0f / 120));
    sn = _mm256_fmadd_ps(u2, sn, _mm256_set1_ps(-1.0f / 6));
    sn = _mm256_fmadd_ps(_mm256_mul_ps(u, u2), sn, u);

    __m256 cs = _mm256_fmadd_ps(u2, _mm256_set1_ps(1.0f / 40320), _mm256_set1_ps(-1.0f / 720));
    cs = _mm256_fmadd_ps(u2, cs, _mm256_set1_ps(1.0f / 24));
    cs = _mm256_fmadd_ps(u2, cs, _mm256_set1_ps(-0.5f));
    cs = _mm256_fmadd_ps(u2, cs, _mm256_set1_ps(1.0f));

    __m256 num = _mm256_fmadd_ps(s0, cs, _mm256_mul_ps(c0, sn));
    __m256 d = _mm256_add_ps(t0, u);
    return _mm256_div_ps(num, d);
}

INLINE float hsum(__m256 v)
{
    float t[8], r = 0;
    _mm256_storeu_ps(t, v);
    for (int i = 0; i < 8; i++)
        r += t[i];
    return r;
}

__attribute__((noinline)) static float block_avx2(double t0, float h)
{
    float s0, c0;
    phase(t0, &s0, &c0);
    __m256 vs0 = _mm256_set1_ps(s0), vc0 = _mm256_set1_ps(c0);
    __m256 vt0 = _mm256_set1_ps((float)t0), vh = _mm256_set1_ps(h);
    __m256 idx = _mm256_setr_ps(0.5f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f);
    __m256 step = _mm256_set1_ps(8.0f), acc = _mm256_setzero_ps();
    for (int j = 0; j < BLOCK; j += 8) {
        acc = _mm256_add_ps(acc, term(_mm256_mul_ps(idx, vh), vs0, vc0, vt0));
        idx = _mm256_add_ps(idx, step);
    }
    return hsum(acc);
}

int main(int argc, char **argv)
{
    run("avx2", block_avx2, argc, argv);
    return 0;
}
