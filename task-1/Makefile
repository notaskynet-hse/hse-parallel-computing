CC = gcc
ARCH = native
ARGS =
VECINFO = -fopt-info-vec-all
LDLIBS = -lm

BINS = pi_O0 pi_O3 pi_semi pi_avx2 pi_avx512
HDR = src/common.h

all: $(BINS)

pi_O0: src/pi_naive.c $(HDR)
	$(CC) -O0 -DSTAGE='"O0"' $< -o $@ $(LDLIBS)

pi_O3: src/pi_naive.c $(HDR)
	$(CC) -O3 -march=$(ARCH) -DSTAGE='"O3"' $< -o $@ $(LDLIBS)

pi_semi: src/pi_semi.c $(HDR)
	$(CC) -O3 -march=$(ARCH) -ffast-math -fopenmp-simd $< -o $@ $(LDLIBS)

pi_avx2: src/pi_avx2.c $(HDR)
	$(CC) -O3 -mavx2 -mfma $< -o $@ $(LDLIBS)

pi_avx512: src/pi_avx512.c $(HDR)
	$(CC) -O3 $< -o $@ $(LDLIBS)

sysinfo:
	mkdir -p results
	{ lscpu | grep -E '^(Model name|CPU\(s\)|Thread|Core|Socket|L1d|L2|L3)'; \
	  echo "SIMD: $$(grep -o -w -E 'sse4_2|avx2|fma|avx512f' /proc/cpuinfo | sort -u | tr '\n' ' ')"; \
	  uname -sr; $(CC) --version | head -1; } > results/sysinfo.txt 2>&1 || true

bench: all
	mkdir -p results
	{ echo stage,time_s,pi,error; \
	  ./pi_O0 $(ARGS); ./pi_O3 $(ARGS); ./pi_semi $(ARGS); \
	  ./pi_avx2 $(ARGS); ./pi_avx512 $(ARGS); } > results/results.csv

define extract
sed -n '/^_*$(1):/,/\.cfi_endproc/p' $(2) | grep -v -E '^\s*(\.cfi|\.p2align|\.loc|\#)' > asm/$(3).txt
endef

asm:
	mkdir -p asm
	$(CC) -O3 -march=$(ARCH) -S -masm=intel src/pi_naive.c -o asm/naive.s
	$(CC) -O3 -march=$(ARCH) -ffast-math -fopenmp-simd -S -masm=intel src/pi_semi.c -o asm/semi.s
	$(CC) -O3 -mavx2 -mfma -S -masm=intel src/pi_avx2.c -o asm/avx2.s
	$(CC) -O3 -S -masm=intel src/pi_avx512.c -o asm/avx512.s
	$(call extract,block_naive,asm/naive.s,naive)
	$(call extract,block_semi,asm/semi.s,semi)
	$(call extract,block_avx2_c,asm/avx2.s,avx2_c)
	$(call extract,block_avx512_b,asm/avx512.s,avx512_b)

vecreport:
	mkdir -p results
	$(CC) -O3 -march=$(ARCH) $(VECINFO) -c src/pi_naive.c -o /dev/null 2> results/vec_naive.txt || true
	$(CC) -O3 -march=$(ARCH) -ffast-math -fopenmp-simd $(VECINFO) -c src/pi_semi.c -o /dev/null 2> results/vec_semi.txt || true

report:
	typst compile --root . report/report.typ report/report.pdf

clean:
	rm -rf $(BINS) asm results report/report.pdf

.PHONY: all sysinfo bench asm vecreport report clean
