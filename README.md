# Accelerating Fuzzing through Prefix-guided Execution (PGE)

This repository contains the implementation of AFL++-PGE used in the evaluation part.

## Contents

  1. [How to build AFL++-PGE](#building-afl-pge)
  2. [How to instrument a target with AFL++-PGE](#instrumenting-binaries)
  3. [How to fuzz a target with AFL++-PGE](#fuzzing)
  4. [How to use AFL++-PGE on Magma](#using-afl-pge-on-magma)


## Building AFL++-PGE 

Clone this repository and cd to the root directory, then
```shell
$ cd AFLplusplus-PGE
$ make all
```

Note that, since AFL++-PGE uses LLVM PCGUARD for instrumentation, we strongly suggest to compile AFL++-PGE with clang >= 13 for better performance.

## Instrumenting Binaries

A nearly-universal approach would be specifying C/C++ compiler when configuring and building your binary:
```shell
$ CC=/path/to/AFLplusplus-PGE/afl-clang-fast CXX=/path/to/AFL-PGE/afl-clang-fast++ ./configure
$ CC=/path/to/AFLplusplus-PGE/afl-clang-fast CXX=/path/to/AFL-PGE/afl-clang-fast++ make clean all
```

## Fuzzing

There is only one extra argument in AFL++-PGE compared to AFL++:
- `-r` (required),the target recall rate used during prefix length search.

The fuzzing command for a binary would be, for example:
```shell
$ /path/to/AFLplusplus-PGE/afl-fuzz -r 90 -m none -i input/ -o output/ -- /path/to/bin @@
```

## Using AFL++-PGE on Magma

If you'd like to evaluate AFL++-PGE on Magma benchmark by yourself, we provide the general steps as well:

1) clone the magma repo;

2) copy the directory in `magma_integration/aflplusplus_pge_r90` into `/path/to/magma/fuzzers/aflplusplus_pge_r90`

The default recall is set to 90%. You can change it in magma_integration/aflplusplus_pge_r90/run.sh

