#include <iostream>


#include "include/profiler.hpp"

int main() {

    char data[256 * 1024];

    auto L1 = std::make_shared<cache<false>>("L1", 64, 32 * 1024); // 32 KiB L1 cache
    auto L2 = std::make_shared<cache<false>>("L2", 64, 256 * 1024); // 256 KiB L2 cache
    auto L3 = std::make_shared<cache<false>>("L3", 64, 16 * 1024 * 1024); // 16 MiB L3 cache

    profiler global_profiiler("GlobalProf", {L1, L2, L3});
    auto branch_1 = global_profiiler.get_branch_profiler("FirstForLoop");

    {
        auto mem = global_profiiler.get_mem_profiler("Mem1");
        auto mem2 = global_profiiler.get_mem_profiler("Mem2");

        for (size_t i = 0; branch_1(i < 1024 * 1024); i++) {
            mem(data + (rand() % 256 * 1024));
        }
        //std::cout << mem << std::endl;

        for (size_t i = 0; i < 1024; i++) {
            mem2(data + (rand() % 32 * 1024));
        }
        std::cout << global_profiiler << std::endl;
    }


    std::cout << *L1 << std::endl;
    //std::cout << global_profiiler << std::endl;

    return 0;
}
