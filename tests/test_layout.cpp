// Tests for block_range and the fill/check helpers.
//   make test       (runs this with several rank counts)
//
// Once the transposes exist, their tests follow the same pattern: fill
// member-major, transpose, then count_state_major_errors must be 0 on every
// rank.

#include <mpi.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "data.hpp"
#include "layout.hpp"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank = 0;
    int nranks = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    int failures = 0;

    auto check = [&](bool ok, const std::string& what) {
        if (!ok) {
            ++failures;
            std::cerr << "rank " << rank << " FAIL: " << what << "\n";
        }
    };

    // --- block_range: blocks must tile [0, total) and differ by <= 1 --------
    // This is pure arithmetic, so each rank runs it independently.
    struct RangeCase { std::size_t total; int parts; };
    const RangeCase range_cases[] = {
        {16, 4}, {17, 3}, {3, 5}, {0, 2}, {1, 1}, {10007, 7},
    };
    for (const auto& c : range_cases) {
        std::size_t next_start = 0;
        std::size_t min_count = c.total;
        std::size_t max_count = 0;
        for (int r = 0; r < c.parts; ++r) {
            const et::Block b = et::block_range(c.total, c.parts, r);
            check(b.start == next_start, "blocks are contiguous");
            next_start = b.start + b.count;
            min_count = std::min(min_count, b.count);
            max_count = std::max(max_count, b.count);
        }
        const std::string label = "total=" + std::to_string(c.total) +
                                  " parts=" + std::to_string(c.parts);
        check(next_start == c.total, "blocks cover everything, " + label);
        check(max_count - min_count <= 1, "block sizes differ by <= 1, " + label);
    }

    // --- fill + check, using the real rank count ----------------------------
    // Includes uneven splits and N < P (some ranks own no members).
    const et::Problem problems[] = {
        {4, 16}, {5, 17}, {1, 3}, {80, 10007},
    };
    for (const auto& prob : problems) {
        const std::string label = "N=" + std::to_string(prob.n_members) +
                                  " S=" + std::to_string(prob.state_size);

        auto mm = et::make_member_major(prob, nranks, rank);
        check(et::count_member_major_errors(prob, nranks, rank, mm) == 0,
              "member-major fill passes its check, " + label);

        auto sm = et::make_state_major(prob, nranks, rank);
        check(et::count_state_major_errors(prob, nranks, rank, sm) == 0,
              "state-major fill passes its check, " + label);

        // Test the tester: a corrupted entry must be caught.
        if (!sm.empty()) {
            sm.back() += 1.0;
            check(et::count_state_major_errors(prob, nranks, rank, sm) == 1,
                  "state-major check catches one bad entry, " + label);
        }
        // A wrong-sized buffer must fail too.
        mm.push_back(0.0);
        check(et::count_member_major_errors(prob, nranks, rank, mm) != 0,
              "member-major check rejects wrong size, " + label);
    }

    // Combine every rank's failure count so rank 0 can report one result.
    int total_failures = 0;
    MPI_Allreduce(&failures, &total_failures, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (rank == 0) {
        std::cout << (total_failures == 0 ? "PASS" : "FAIL") << " test_layout (np="
                  << nranks << ", failures=" << total_failures << ")\n";
    }

    MPI_Finalize();
    return total_failures == 0 ? 0 : 1;
}
