CC = gcc
ARCH = native
ARGS =
VECINFO = -fopt-info-vec-all
LDLIBS = -lm

B = build
BINS = $(B)/pi_O0 $(B)/pi_O3 $(B)/pi_semi $(B)/pi_avx2 $(B)/pi_avx512
HDR = src/common.h

all: $(BINS)

$(B):
	mkdir -p $@

$(B)/pi_O0: src/pi_naive.c $(HDR) | $(B)
	$(CC) -O0 -DSTAGE='"O0"' $< -o $@ $(LDLIBS)

$(B)/pi_O3: src/pi_naive.c $(HDR) | $(B)
	$(CC) -O3 -march=$(ARCH) -DSTAGE='"O3"' $< -o $@ $(LDLIBS)

$(B)/pi_semi: src/pi_semi.c $(HDR) | $(B)
	$(CC) -O3 -march=$(ARCH) -ffast-math -fopenmp-simd $< -o $@ $(LDLIBS)

$(B)/pi_avx2: src/pi_avx2.c $(HDR) | $(B)
	$(CC) -O3 -mavx2 -mfma $< -o $@ $(LDLIBS)

$(B)/pi_avx512: src/pi_avx512.c $(HDR) | $(B)
	$(CC) -O3 $< -o $@ $(LDLIBS)

sysinfo: | $(B)
	{ lscpu | grep -E '^(Model name|CPU\(s\)|Thread|Core|Socket|L1d|L2|L3)'; \
	  echo "SIMD: $$(grep -o -w -E 'sse4_2|avx2|fma|avx512f' /proc/cpuinfo | sort -u | tr '\n' ' ')"; \
	  uname -sr; $(CC) --version | head -1; } > $(B)/sysinfo.txt 2>&1 || true

bench: all
	{ echo stage,time_s,pi,error; \
	  $(B)/pi_O0 $(ARGS); $(B)/pi_O3 $(ARGS); $(B)/pi_semi $(ARGS); \
	  $(B)/pi_avx2 $(ARGS); $(B)/pi_avx512 $(ARGS); } > $(B)/results.csv

define extract
sed -n '/^_*$(1):/,/\.cfi_endproc/p' $(B)/$(2).s | grep -v -E '^\s*(\.cfi|\.p2align|\.loc|\#)' > $(B)/$(3).txt
endef

asm: | $(B)
	$(CC) -O3 -march=$(ARCH) -S -masm=intel src/pi_naive.c -o $(B)/naive.s
	$(CC) -O3 -march=$(ARCH) -ffast-math -fopenmp-simd -S -masm=intel src/pi_semi.c -o $(B)/semi.s
	$(CC) -O3 -mavx2 -mfma -S -masm=intel src/pi_avx2.c -o $(B)/avx2.s
	$(CC) -O3 -S -masm=intel src/pi_avx512.c -o $(B)/avx512.s
	$(call extract,block_naive,naive,naive)
	$(call extract,block_semi,semi,semi)
	$(call extract,block_avx2_c,avx2,avx2_c)
	$(call extract,block_avx512_b,avx512,avx512_b)

vecreport: | $(B)
	$(CC) -O3 -march=$(ARCH) $(VECINFO) -c src/pi_naive.c -o /dev/null 2> $(B)/vec_naive.txt || true
	$(CC) -O3 -march=$(ARCH) -ffast-math -fopenmp-simd $(VECINFO) -c src/pi_semi.c -o /dev/null 2> $(B)/vec_semi.txt || true

report: | $(B)
	typst compile --root . report/report.typ $(B)/report.pdf

clean:
	rm -rf $(B)

.PHONY: all sysinfo bench asm vecreport report clean
