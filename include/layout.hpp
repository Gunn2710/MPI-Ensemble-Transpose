// How the ensemble is split across ranks.
//
// An ensemble is N members, each a state vector of S doubles. Both layouts
// divide something into P near-equal contiguous blocks:
//   member-major: the N members are divided over P ranks
//   state-major:  the S state indices are divided over P ranks
// block_range() is the single rule both layouts use for that.

#pragma once

#include <cstddef>

namespace et {

// The size of one ensemble. N = n_members, S = state_size.
struct Problem {
    std::size_t n_members;
    std::size_t state_size;
};

// A contiguous range [start, start + count) of a total.
struct Block {
    std::size_t start;
    std::size_t count;
};

// Which part of `total` items rank `part` (of `nparts`) owns.
// The first (total % nparts) ranks get one extra item, so counts differ by at
// most 1 and the blocks cover [0, total) in order with no gaps or overlap.
// A rank can own zero items when total < nparts.
Block block_range(std::size_t total, int nparts, int part);

}  // namespace et
