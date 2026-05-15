# Delete any results from prior runs
rm -rf results.md

# Compile all three
clang++ -std=c++23 -O2 -o bench_v1.o bench_v1.cpp
clang++ -std=c++23 -O2 -o bench_v2.o bench_v2.cpp
clang++ -std=c++23 -O2 -o bench_stdlib.o bench_stdlib.cpp

# Benchmark
hyperfine --warmup 5 \
  --runs 50 \
  --export-markdown results.md \
  -n 'if-conditionals (v1)'        './bench_v1.o' \
  -n 'function pointer table (v2)' './bench_v2.o' \
  -n 'stdlib variant'              './bench_stdlib.o'