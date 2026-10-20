#include <immintrin.h>

#include "common.h"

#define INLINE static inline __attribute__((always_inline))

INLINE __m256 term(__m256 u, __m256 s0, __m256 c0, __m256 t0, int fast)
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
    if (!fast)
        return _mm256_div_ps(num, d);

    __m256 r = _mm256_rcp_ps(d);
    r = _mm256_mul_ps(r, _mm256_fnmadd_ps(d, r, _mm256_set1_ps(2.0f)));
    return _mm256_mul_ps(num, r);
}

INLINE float hsum(__m256 v)
{
    float t[8], r = 0;
    _mm256_storeu_ps(t, v);
    for (int i = 0; i < 8; i++)
        r += t[i];
    return r;
}

INLINE float kernel(double t0, float h, int four, int fast)
{
    float s0, c0;
    phase(t0, &s0, &c0);
    __m256 vs0 = _mm256_set1_ps(s0), vc0 = _mm256_set1_ps(c0);
    __m256 vt0 = _mm256_set1_ps((float)t0), vh = _mm256_set1_ps(h);
    __m256 idx = _mm256_setr_ps(0.5f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f);
    __m256 a0 = _mm256_setzero_ps(), a1 = a0, a2 = a0, a3 = a0;

    if (!four) {
        __m256 step = _mm256_set1_ps(8.0f);
        for (int j = 0; j < BLOCK; j += 8) {
            a0 = _mm256_add_ps(a0, term(_mm256_mul_ps(idx, vh), vs0, vc0, vt0, fast));
            idx = _mm256_add_ps(idx, step);
        }
        return hsum(a0);
    }

    __m256 e8 = _mm256_set1_ps(8.0f), e16 = _mm256_set1_ps(16.0f);
    __m256 e24 = _mm256_set1_ps(24.0f), e32 = _mm256_set1_ps(32.0f);
    for (int j = 0; j < BLOCK; j += 32) {
        __m256 u0 = _mm256_mul_ps(idx, vh);
        __m256 u1 = _mm256_mul_ps(_mm256_add_ps(idx, e8), vh);
        __m256 u2 = _mm256_mul_ps(_mm256_add_ps(idx, e16), vh);
        __m256 u3 = _mm256_mul_ps(_mm256_add_ps(idx, e24), vh);
        a0 = _mm256_add_ps(a0, term(u0, vs0, vc0, vt0, fast));
        a1 = _mm256_add_ps(a1, term(u1, vs0, vc0, vt0, fast));
        a2 = _mm256_add_ps(a2, term(u2, vs0, vc0, vt0, fast));
        a3 = _mm256_add_ps(a3, term(u3, vs0, vc0, vt0, fast));
        idx = _mm256_add_ps(idx, e32);
    }
    return hsum(_mm256_add_ps(_mm256_add_ps(a0, a1), _mm256_add_ps(a2, a3)));
}

__attribute__((noinline)) static float block_avx2_a(double t0, float h)
{
    return kernel(t0, h, 0, 0);
}

__attribute__((noinline)) static float block_avx2_b(double t0, float h)
{
    return kernel(t0, h, 1, 0);
}

__attribute__((noinline)) static float block_avx2_c(double t0, float h)
{
    return kernel(t0, h, 1, 1);
}

int main(int argc, char **argv)
{
    run("avx2_a", block_avx2_a, argc, argv);
    run("avx2_b", block_avx2_b, argc, argv);
    run("avx2_c", block_avx2_c, argc, argv);
    return 0;
}
