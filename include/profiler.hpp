#pragma once

#include "internal/cache.hpp"
#include "internal/memory_profiler.hpp"
#include "internal/branch_profiler.hpp"
#include <list>
#include <ostream>


template<bool thread_safe = false>
class profiler {
private:
    std::string name_;
    std::vector<cache_layer<thread_safe>> caches_;
    std::list<memory_profiler<thread_safe> *> mem_profilers{};
    std::list<branch_profiler<thread_safe> *> branch_profilers{};

public:
    friend std::ostream &operator<<(std::ostream &os, const profiler<thread_safe> &profiler) {
        os << "************************************ Profiler Report ************************************\n" << " - name_: " << profiler.name_ << "\n\n";
        for (const cache_layer<thread_safe> &c : profiler.caches_) {
            os << *(c.a_cache_) << '\n';
        }

        for (const memory_profiler<thread_safe> *c : profiler.mem_profilers) {
            os << *c << '\n';
        }

        for (const branch_profiler<thread_safe> *c : profiler.branch_profilers) {
            os << *c << '\n';
        }


        os << "***************************************** END *******************************************\n";
        return os;
    }

public:
    explicit profiler(std::string name, const std::vector<std::shared_ptr<cache<thread_safe>>> &caches) :
            name_(std::move(name)) {
        for (auto &cache : caches) {
            caches_.push_back({cache, 0});
        }
    }

    profiler(const profiler &) = delete;

    profiler &operator=(const profiler) = delete;

    profiler &operator=(profiler &&other) = delete;


    memory_profiler<thread_safe> get_mem_profiler(std::string name) {
        return memory_profiler(std::move(name), caches_, this);
    }

    branch_profiler<thread_safe> get_branch_profiler(std::string name) {
        return branch_profiler(std::move(name), this);
    }

    void register_mem_profiler(memory_profiler<thread_safe> *prof) {
        mem_profilers.push_back(prof);
    }

    void unregister_mem_profiler(memory_profiler<thread_safe> *prof) {
        mem_profilers.remove(prof);
    }

    void register_branch_profiler(branch_profiler<thread_safe> *prof) {
        branch_profilers.push_back(prof);
    }

    void unregister_branch_profiler(branch_profiler<thread_safe> *prof) {
        branch_profilers.remove(prof);
    }
};