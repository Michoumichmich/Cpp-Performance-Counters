#pragma once

#include "cache.hpp"
#include "config.hpp"
#include <memory>
#include <utility>
#include <array>
#include <ostream>


template<bool thread_safe>
class profiler;

template<bool thread_safe = false>
class memory_profiler {
private:
    std::mutex state_mutex_;
    std::string name_;
    std::vector<cache_layer<thread_safe>> caches_;
    profiler<thread_safe> *parent_;

    size_t main_memory_accesses = 0;
    size_t memory_accesses_ = 0;
    size_t memory_read_ = 0;

public:
    explicit memory_profiler(std::string name, std::vector<cache_layer<thread_safe>> caches, profiler<thread_safe> *parent_prof) :
            name_(std::move(name)), caches_(std::move(caches)), parent_(parent_prof) {
        parent_->register_mem_profiler(this);
    }

    memory_profiler(const memory_profiler &) = delete;

    memory_profiler &operator=(const memory_profiler) = delete;

    memory_profiler &operator=(memory_profiler &&other) = delete;

    virtual ~memory_profiler() {
        parent_->unregister_mem_profiler(this);
    }

private:

    template<typename T>
    T &access_ptr(T *ptr) {
        ++memory_accesses_;
        memory_read_ += sizeof(T);

        for (cache_layer<thread_safe> &c :caches_) {
            auto access_misses = c.a_cache_->template access_data<T>(ptr);
            c.cache_access_counter_++;
            if (!access_misses) {
                return *ptr;
            }
            c.cache_miss_counter_ += access_misses;
        }

        ++main_memory_accesses;
        return *ptr;
    }


public:

    template<typename T>
    inline T &operator()(T *ptr) {
        if constexpr(enable_profilers) {
            if constexpr(thread_safe) {
                std::lock_guard m(state_mutex_);
                return access_ptr(ptr);
            } else {
                return access_ptr(ptr);
            }
        } else {
            return *ptr;
        }

    }

    friend std::ostream &operator<<(std::ostream &os, const memory_profiler &profiler) {
        os << " - Memory report for: " << profiler.name_ << ":\n";
        if (!profiler.memory_accesses_) {
            os << "No memory accessed \n";

        } else {
            os << "    - memory_accesses: " << profiler.memory_accesses_ << ", data_read: " << profiler.memory_read_ << " bytes.\n";
            for (const cache_layer<thread_safe> &c : profiler.caches_) {
                os << "    - " << c.a_cache_->getName() << "_miss_counter: " << c.cache_miss_counter_
                   << ", memory_accesses: " << c.cache_access_counter_
                   << ", miss_rate: " << 100 * (double) c.cache_miss_counter_ / (double) c.cache_access_counter_ << " %"
                   << ", data_read/data_loaded: " << 100 * (double) (profiler.memory_read_) / (double) (c.cache_miss_counter_ * c.a_cache_->getCacheLineSize()) << " %.\n";
            }
            os << "    - main_memory_accesses: " << profiler.main_memory_accesses << '\n';
        }

        return os;
    }

};




