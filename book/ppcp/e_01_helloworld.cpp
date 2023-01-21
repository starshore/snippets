/*********************************************************************
 * \file   e_01_helloworld.cpp
 * \brief  first parallel program
 *
 * \author starshore
 * \date   January 2023
 *********************************************************************/

#include <stdafx.h>

#include <thread>
#include <vector>

TEST_CASE("hello world")
{
    const size_t             num_threads = 4;
    std::vector<std::thread> threads;

    for (size_t id = 0; id != num_threads; ++id) {
        threads.emplace_back([](auto &&id) { spdlog::info("hello from thread: {}", id); }, id);
    }

    for (auto &&t : threads) {
        t.join();
    }
}
