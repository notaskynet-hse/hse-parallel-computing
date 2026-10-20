#include <immintrin.h>

#include "common.h"

#define TARGET __attribute__((target("avx512f")))
#define INLINE static inline __attribute__((always_inline, target("avx512f")))

INLINE __m512 term(__m512 u, __m512 s0, __m512 c0, __m512 t0)
{
    __m512 u2 = _mm512_mul_ps(u, u);

    __m512 sn = _mm512_fmadd_ps(u2, _mm512_set1_ps(1.0f / 362880), _mm512_set1_ps(-1.0f / 5040));
    sn = _mm512_fmadd_ps(u2, sn, _mm512_set1_ps(1.0f / 120));
    sn = _mm512_fmadd_ps(u2, sn, _mm512_set1_ps(-1.0f / 6));
    sn = _mm512_fmadd_ps(_mm512_mul_ps(u, u2), sn, u);

    __m512 cs = _mm512_fmadd_ps(u2, _mm512_set1_ps(1.0f / 40320), _mm512_set1_ps(-1.0f / 720));
    cs = _mm512_fmadd_ps(u2, cs, _mm512_set1_ps(1.0f / 24));
    cs = _mm512_fmadd_ps(u2, cs, _mm512_set1_ps(-0.5f));
    cs = _mm512_fmadd_ps(u2, cs, _mm512_set1_ps(1.0f));

    __m512 num = _mm512_fmadd_ps(s0, cs, _mm512_mul_ps(c0, sn));
    __m512 d = _mm512_add_ps(t0, u);
    return _mm512_div_ps(num, d);
}

__attribute__((noinline)) TARGET static float block_avx512(double t0, float h)
{
    float s0, c0;
    phase(t0, &s0, &c0);
    __m512 vs0 = _mm512_set1_ps(s0), vc0 = _mm512_set1_ps(c0);
    __m512 vt0 = _mm512_set1_ps((float)t0), vh = _mm512_set1_ps(h);
    __m512 idx = _mm512_add_ps(
        _mm512_cvtepi32_ps(_mm512_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)),
        _mm512_set1_ps(0.5f));
    __m512 a0 = _mm512_setzero_ps(), a1 = a0, a2 = a0, a3 = a0;

    __m512 e16 = _mm512_set1_ps(16.0f), e32 = _mm512_set1_ps(32.0f);
    __m512 e48 = _mm512_set1_ps(48.0f), e64 = _mm512_set1_ps(64.0f);
    for (int j = 0; j < BLOCK; j += 64) {
        __m512 u0 = _mm512_mul_ps(idx, vh);
        __m512 u1 = _mm512_mul_ps(_mm512_add_ps(idx, e16), vh);
        __m512 u2 = _mm512_mul_ps(_mm512_add_ps(idx, e32), vh);
        __m512 u3 = _mm512_mul_ps(_mm512_add_ps(idx, e48), vh);
        a0 = _mm512_add_ps(a0, term(u0, vs0, vc0, vt0));
        a1 = _mm512_add_ps(a1, term(u1, vs0, vc0, vt0));
        a2 = _mm512_add_ps(a2, term(u2, vs0, vc0, vt0));
        a3 = _mm512_add_ps(a3, term(u3, vs0, vc0, vt0));
        idx = _mm512_add_ps(idx, e64);
    }
    return _mm512_reduce_add_ps(_mm512_add_ps(_mm512_add_ps(a0, a1), _mm512_add_ps(a2, a3)));
}

int main(int argc, char **argv)
{
    __builtin_cpu_init();
    if (!__builtin_cpu_supports("avx512f"))
        return 0;
    run("avx512", block_avx512, argc, argv);
    return 0;
}
