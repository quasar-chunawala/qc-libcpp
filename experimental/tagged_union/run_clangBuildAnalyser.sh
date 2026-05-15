#!/usr/bin/env bash
rm -rf traces/v1 traces/v2 traces/stdlib
rm -rf capture_v1.bin capture_v2.bin capture_stdlib.bin
rm -rf report_v1.txt report_v2.txt report_stdlib.txt
mkdir -p traces/v1 traces/v2 traces/stdlib

clang++ -std=c++23 -ftime-trace -o traces/v1/bench_v1.o    -c bench_v1.cpp
clang++ -std=c++23 -ftime-trace -o traces/v2/bench_v2.o    -c bench_v2.cpp
clang++ -std=c++23 -ftime-trace -o traces/stdlib/bench_stdlib.o -c bench_stdlib.cpp

ClangBuildAnalyzer --all traces/v1     capture_v1.bin
ClangBuildAnalyzer --all traces/v2     capture_v2.bin
ClangBuildAnalyzer --all traces/stdlib capture_stdlib.bin

ClangBuildAnalyzer --analyze capture_v1.bin     > report_v1.txt
ClangBuildAnalyzer --analyze capture_v2.bin     > report_v2.txt
ClangBuildAnalyzer --analyze capture_stdlib.bin > report_stdlib.txt