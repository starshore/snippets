/*********************************************************************
 * \file   e_03_dispatch_param.cpp
 * \brief  dispatch param
 *
 * \author starshore
 * \date   January 2023
 *********************************************************************/

#include <stdafx.h>

#include <vector>
#include <thread>

#include "hpc_helpers.hpp"

template <typename value_t, typename index_t>
void init(std::vector<value_t> &A, std::vector<value_t> &x, index_t m, index_t n)
{
    for (index_t row = 0; row != m; ++row) {
        for (index_t col = 0; col != n; ++col) {
            A[row * n + col] = row > col ? 1 : 0;
        }
    }

    for (index_t col = 0; col < n; ++col) {
        x[col] = col;
    }
}

template <typename value_t, typename index_t>
void sequential_mult(std::vector<value_t> &A,
                     std::vector<value_t> &x,
                     std::vector<value_t> &b,
                     index_t               m,
                     index_t               n)
{

    for (index_t row = 0; row < m; row++) {
        value_t accum = value_t(0);
        for (index_t col = 0; col < n; col++)
            accum += A[row * n + col] * x[col];
        b[row] = accum;
    }
}

TEST_CASE("e_03_sequential_mult")
{
    spdlog::info("--- --- --- e_03_sequential_mult --- --- ---");

    const uint64_t n = (1UL << 15);
    const uint64_t m = (1UL << 15);

    TIMERSTART(overall);

    std::vector<uint64_t> A(m * n);
    std::vector<uint64_t> x(n);
    std::vector<uint64_t> b(m);
    init(A, x, m, n);

    TIMERSTART(mult);
    sequential_mult(A, x, b, m, n);
    TIMERSTOP(mult);

    TIMERSTOP(overall);
}

template <typename value_t, typename index_t>
void block_parallel_mult(std::vector<value_t> &A,
                         std::vector<value_t> &x,
                         std::vector<value_t> &b,
                         index_t               m,
                         index_t               n,
                         index_t               num_threads = 8)
{
    auto block = [&](const index_t &id) -> void {
        const index_t chunk = SDIV(m, num_threads);
        const index_t lower = id * chunk;
        const index_t upper = std::min(lower + chunk, m);

        for (index_t row = lower; row < upper; row++) {
            value_t accum = value_t(0);
            for (index_t col = 0; col < n; col++)
                accum += A[row * n + col] * x[col];
            b[row] = accum;
        }
    };

    std::vector<std::thread> threads;

    for (index_t id = 0; id < num_threads; id++)
        threads.emplace_back(block, id);

    for (auto &thread : threads)
        thread.join();
}

TEST_CASE("e_03_block_parallel_mult")
{
    spdlog::info("--- --- --- e_03_block_parallel_mult --- --- ---");

    const uint64_t n = (1UL << 15);
    const uint64_t m = (1UL << 15);

    TIMERSTART(overall);

    std::vector<uint64_t> A(m * n);
    std::vector<uint64_t> x(n);
    std::vector<uint64_t> b(m);
    init(A, x, m, n);

    TIMERSTART(mult);
    block_parallel_mult(A, x, b, m, n);
    TIMERSTOP(mult);

    TIMERSTOP(overall);
}

template <typename value_t, typename index_t>
void cyclic_parallel_mult(std::vector<value_t> &A,
                          std::vector<value_t> &x,
                          std::vector<value_t> &b,
                          index_t               m,
                          index_t               n,
                          index_t               num_threads = 8)
{
    auto cyclic = [&](const index_t &id) -> void {
        for (index_t row = id; row < m; row += num_threads) {
            value_t accum = value_t(0);
            for (index_t col = 0; col < n; col++) {
                // false sharing!!
                // b[row] += A[row * n + col] * x[col];

                accum += A[row * n + col] * x[col];
            }
            b[row] = accum;
        }
    };

    std::vector<std::thread> threads;

    for (index_t id = 0; id < num_threads; id++)
        threads.emplace_back(cyclic, id);

    for (auto &thread : threads)
        thread.join();
}

TEST_CASE("e_03_cyclic_parallel_mult")
{
    spdlog::info("--- --- --- e_03_cyclic_parallel_mult --- --- ---");

    const uint64_t n = (1UL << 15);
    const uint64_t m = (1UL << 15);

    TIMERSTART(overall);

    std::vector<uint64_t> A(m * n);
    std::vector<uint64_t> x(n);
    std::vector<uint64_t> b(m);
    init(A, x, m, n);

    TIMERSTART(mult);
    cyclic_parallel_mult(A, x, b, m, n);
    TIMERSTOP(mult);

    TIMERSTOP(overall);
}
