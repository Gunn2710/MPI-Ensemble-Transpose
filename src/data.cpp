#include "data.hpp"

#include <algorithm>

namespace et {

double expected_value(const Problem& prob, std::size_t member, std::size_t i) {
    return static_cast<double>(member * prob.state_size + i);
}

std::vector<double> make_member_major(const Problem& prob, int nranks, int rank) {
    const Block mine = block_range(prob.n_members, nranks, rank);
    const std::size_t S = prob.state_size;

    std::vector<double> buf(mine.count * S);
    for (std::size_t m = 0; m < mine.count; ++m) {
        for (std::size_t i = 0; i < S; ++i) {
            buf[m * S + i] = expected_value(prob, mine.start + m, i);
        }
    }
    return buf;
}

std::vector<double> make_state_major(const Problem& prob, int nranks, int rank) {
    const Block mine = block_range(prob.state_size, nranks, rank);
    const std::size_t N = prob.n_members;

    std::vector<double> buf(N * mine.count);
    for (std::size_t m = 0; m < N; ++m) {
        for (std::size_t i = 0; i < mine.count; ++i) {
            buf[m * mine.count + i] = expected_value(prob, m, mine.start + i);
        }
    }
    return buf;
}

std::size_t count_member_major_errors(const Problem& prob, int nranks, int rank,
                                      std::span<const double> buf) {
    const Block mine = block_range(prob.n_members, nranks, rank);
    const std::size_t S = prob.state_size;
    if (buf.size() != mine.count * S) return std::max(buf.size(), mine.count * S);

    std::size_t errors = 0;
    for (std::size_t m = 0; m < mine.count; ++m) {
        for (std::size_t i = 0; i < S; ++i) {
            // Exact == is fine here: values are copied around, never computed.
            if (buf[m * S + i] != expected_value(prob, mine.start + m, i)) ++errors;
        }
    }
    return errors;
}

std::size_t count_state_major_errors(const Problem& prob, int nranks, int rank,
                                     std::span<const double> buf) {
    const Block mine = block_range(prob.state_size, nranks, rank);
    const std::size_t N = prob.n_members;
    if (buf.size() != N * mine.count) return std::max(buf.size(), N * mine.count);

    std::size_t errors = 0;
    for (std::size_t m = 0; m < N; ++m) {
        for (std::size_t i = 0; i < mine.count; ++i) {
            if (buf[m * mine.count + i] != expected_value(prob, m, mine.start + i)) ++errors;
        }
    }
    return errors;
}

}  // namespace et
