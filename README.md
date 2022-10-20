# Accelerating Fuzzing through Prefix-guided Execution (PGE)

This repository contains the implementation of AFL++-PGE and raw experimental data used in the evaluation part.

## Contents

  1. [How to build AFL++-PGE](#building-afl-pge)
  2. [How to instrument a target with AFL++-PGE](#instrumenting-binaries)
  3. [How to fuzz a target with AFL++-PGE](#fuzzing)
  4. [How to use AFL++-PGE on Magma](#using-afl-pge-on-magma)
  5. [Raw experimental results on Magma](#evaluation-results-of-afl-pge-on-magma)


## Building AFL-PGE 

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
$ /path/to/AFLplusplus-PGE/afl-fuzz -r 70 -m none -i input/ -o output/ -- /path/to/bin @@
```

## Using AFL++-PGE on Magma

If you'd like to evaluate AFL++-PGE on Magma benchmark by yourself, we provide the general steps as well:

1) clone the magma repo;
2) copy the directory in magma_integration/aflplusplus_pge_r90 into /path/to/magma/fuzzers/aflplusplus_pge_r90

The default recall is set to 90%. You can change it in magma_integration/aflplusplus_pge_r90/run.sh

## Evaluation results of AFL++-PGE on Magma

We have evaluated AFL-PGE on Magma v1.2 in 72 hours fuzzing campaign. The subdirectory `evaluation/` contains all raw experimental results produced by Magma.

- `evaluation/magma_benchd_72h/` contains all json outputs produced by `magma/toos/benchd/exp2json.py` script, which extracts core results from abundant magma logs.
- `evaluation/magma_report_72h/` contains reports produced by `magma/toos/report_df/main.py` with configuration `config={'duration': 3 * 24 * 60 * 60, 'trials': 10})`.
- `evaluation/magma_coverage_72h/` contains merged coverage information produced by llvm-cov.


