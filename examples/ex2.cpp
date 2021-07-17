#include <iostream>
#include "../include/profiler.hpp"


constexpr int n_iter = 10;
constexpr int matrix_size = 128;
using T = int;

T A[matrix_size * matrix_size];
T B[matrix_size * matrix_size];
T Res[matrix_size * matrix_size];


int main() {

    auto L1 = std::make_shared<cache<false>>("L1", 64, 32 * 1024); // 32 KiB L1 cache
    auto L2 = std::make_shared<cache<false>>("L2", 64, 256 * 1024); // 256 KiB L2 cache
    auto L3 = std::make_shared<cache<false>>("L3", 64, 16 * 1024 * 1024); // 16 MiB L3 cache

    profiler global_profiler("GlobalProf", {L1, L2, L3});
    auto init = global_profiler.get_mem_profiler("Initialising matrices");

    for (int i = 0; i < matrix_size * matrix_size; ++i) {
        init(A + i) = 1;
        init(B + i) = 1;
        init(Res + i) = 0;
    }


    {
        auto p = global_profiler.get_mem_profiler("IJK");
        for (int c = 0; c < n_iter; ++c) {
            for (int i = 0; i < matrix_size; ++i) {
                for (int j = 0; j < matrix_size; ++j) {
                    for (int k = 0; k < matrix_size; ++k) {
                        p(Res + i + matrix_size * j) += p(A + i + matrix_size * k) * p(B + k + matrix_size * j);
                    }
                }
            }
        }

        std::cout << p << std::endl;
    }

    {
        auto p = global_profiler.get_mem_profiler("IKJ");
        for (int c = 0; c < n_iter; ++c) {
            for (int i = 0; i < matrix_size; ++i) {
                for (int k = 0; k < matrix_size; ++k) {
                    for (int j = 0; j < matrix_size; ++j) {
                        p(Res + i + matrix_size * j) += p(A + i + matrix_size * k) * p(B + k + matrix_size * j);
                    }
                }
            }
        }
        std::cout << p << std::endl;
    }

    {
        auto p = global_profiler.get_mem_profiler("JKI");
        for (int c = 0; c < n_iter; ++c) {
            for (int j = 0; j < matrix_size; ++j) {
                for (int k = 0; k < matrix_size; ++k) {
                    for (int i = 0; i < matrix_size; ++i) {
                        p(Res + i + matrix_size * j) += p(A + i + matrix_size * k) * p(B + k + matrix_size * j);
                    }
                }
            }
        }
        std::cout << p << std::endl;
    }


    std::cout << global_profiler << std::endl;
    return 0;
}
