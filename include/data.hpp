// Deterministic test data and correctness checks.
//
// Every entry has a known value that depends only on its global position
// (member m, state index i). Any rank can compute what any entry should hold
// without communicating, so checking a transpose is a local loop.
//
// Local buffer layouts (both row-major, one flat std::vector<double>):
//
//   member-major: rank owns members  M = block_range(N, P, rank)
//                 buf[m_local * S + i]               size = M.count * S
//
//   state-major:  rank owns indices  I = block_range(S, P, rank), all members
//                 buf[m * I.count + i_local]         size = N * I.count
//
// Note that in state-major each member's slice is contiguous. That makes the
// transpose a matter of moving whole [member][slice] blocks.

#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "layout.hpp"

namespace et {

// The value stored at (member, i): its global flat index m * S + i.
// It's unique for every entry and is exact as a double up to 2^53.
double expected_value(const Problem& prob, std::size_t member, std::size_t i);

// Build this rank's buffer with every entry set to expected_value.
std::vector<double> make_member_major(const Problem& prob, int nranks, int rank);
std::vector<double> make_state_major(const Problem& prob, int nranks, int rank);

// Count entries in `buf` that don't match expected_value. 0 means correct.
// A buffer of the wrong size returns max(actual, expected) size, which is
// never 0, even for a rank that owns nothing.
std::size_t count_member_major_errors(const Problem& prob, int nranks, int rank,
                                      std::span<const double> buf);
std::size_t count_state_major_errors(const Problem& prob, int nranks, int rank,
                                     std::span<const double> buf);

}  // namespace et
