/*********************************************************************
 * \file   e_02_promise_feature.cpp
 * \brief  promise and future
 *         s = (p,f); p is writable, future is readable.
 *
 * \author starshore
 * \date   January 2023
 *********************************************************************/

#include <stdafx.h>

#include <thread>
#include <future>
#include <vector>

void fibonacci(int n, std::promise<int> &&result)
{
    int a = 0;
    int b = 1;

    for (int i = 0; i != n; ++i) {

        const int tmp = a;

        a = b;
        b += tmp;
    }

    result.set_value(a);
}

TEST_CASE("e_02_promise_future")
{
    spdlog::info("--- --- --- e_02_promise_future --- --- ---");

    const int                     num_threads = 32;
    std::vector<std::thread>      threads;
    std::vector<std::future<int>> futures;

    for (int i = 0; i != num_threads; ++i) {

        std::promise<int> promise;
        futures.emplace_back(promise.get_future());

        threads.emplace_back(fibonacci, i, std::move(promise));
    }

    for (auto &&future : futures) {
        spdlog::info("future: {}", future.get());
    }

    for (auto &&t : threads) {
        t.join();
    }
}
