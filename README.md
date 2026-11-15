# MPI Ensemble Transpose Benchmark

A small C++/MPI benchmark that reproduces the "transpose" at the heart of ensemble data assimilation (as in NSF NCAR's DART): switching a set of ensemble members between two distributed layouts, and measuring which communication strategy scales best.

> Status: scaffolding only. No implementation yet.

## Background

An ensemble of N model states (each a vector of S doubles) is spread across P MPI ranks, and the layout switches between:

- **Member-major:** each rank holds complete copies of a few ensemble members.
- **State-major:** each rank holds a slice of the state vector across every member.

Switching layouts is an all-to-all exchange. This project builds it several ways and compares them.

## Language

Written in modern C++ (C++20), built with `mpicxx`. MPI has no maintained C++ bindings (they were removed in MPI-3), so the code calls the standard C API and wraps it in small C++ helpers: RAII types that free communicators and datatypes automatically, `std::vector` for buffers instead of `malloc`, and `std::span` to pass views of them around.

## Planned strategies

| ID | Approach | MPI calls |
| --- | --- | --- |
| A | Collective | `MPI_Alltoallv` + manual pack/unpack |
| B | Point-to-point | `MPI_Isend` / `MPI_Irecv` + `MPI_Waitall` |
| C | Derived datatypes | `MPI_Type_create_subarray` + `MPI_Alltoallw` |
| Stretch | Parallel IO | `MPI_File_read_all` vs `MPI_File_iread` overlapped with compute |

## Planned experiments

1. Strong scaling over P (single node and multi-node)
2. Message size sweep (S from ~1e4 to ~1e7 doubles)
3. Ensemble size sweep (N = 20, 40, 80)

Metrics: wall time (median of 10+ runs, max across ranks), effective bandwidth, pack/unpack vs communication time.

## Layout

```
src/        transpose implementations and benchmark driver (.cpp)
include/    shared headers (.hpp)
tests/      correctness tests (uneven N, S, P included)
scripts/    Slurm job scripts for the experiments
analysis/   Python scripts that turn CSVs into plots
results/    summarized CSVs and plots
docs/       project scope and notes
```

## Build and run

Not yet implemented. Intended usage:

```
make            # build with mpicxx (C++20)
make test       # run correctness tests
sbatch scripts/<experiment>.sbatch
```

## Roadmap

- [x] Data model + deterministic fill + correctness check
- [ ] Strategy A (Alltoallv)
- [ ] Strategy B (Isend/Irecv)
- [ ] Strategy C (derived datatypes)
- [ ] Timing harness and CSV output
- [ ] Slurm scripts, cluster runs
- [ ] Plots and findings
- [ ] Stretch: MPI-IO comparison

## Future work

Parked ideas go here (GPU layouts, AMReX grids, real DART integration).
