#include "layout.hpp"

#include <algorithm>
#include <cassert>

namespace et {

Block block_range(std::size_t total, int nparts, int part) {
    assert(nparts > 0 && part >= 0 && part < nparts);

    const auto p = static_cast<std::size_t>(nparts);
    const auto r = static_cast<std::size_t>(part);

    const std::size_t base = total / p;
    const std::size_t extra = total % p;  // the first `extra` ranks get base+1

    const std::size_t count = base + (r < extra ? 1 : 0);
    // Ranks before r own r*base items, plus one extra each for those of them
    // that are among the first `extra` ranks.
    const std::size_t start = r * base + std::min(r, extra);

    return {start, count};
}

}  // namespace et
