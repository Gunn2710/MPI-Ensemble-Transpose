# Project scope

Full scope lives in the shared doc: MPI Ensemble Transpose Benchmark - Project Scope. Key points below. The shared doc still says C; this file is current: the project is in C++.

## Goals

1. Implement the member-major <-> state-major transpose three ways; all must give identical results.
2. Measure time and bandwidth across ranks, ensemble sizes and state sizes, and explain where the time goes.
3. Run scaling experiments on a real cluster through Slurm.
4. Publish a clean repo with a README, plots and 2-3 concrete findings.

## Non-goals

- No data assimilation math (synthetic state arrays).
- No GPU code in the core project.
- No attempt to match DART's real performance numbers.
- No C++ for its own sake: no heavy templates or class hierarchies, and no libraries beyond MPI and the standard library (Boost.MPI included).

## Scope cuts if time runs short

Drop strategy C and the IO stretch first. Strategies A and B plus one scaling plot is still a complete project. Never skip the correctness test or the write-up.
