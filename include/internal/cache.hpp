#pragma once

#include <utility>
#include <vector>
#include <bit>
#include <ostream>


/**
 * @param in
 * @param start_bit inclusive, start from 0
 * @param count
 * @return
 */
size_t extract_bits(size_t in, uint start_bit, uint count) {
    return (in >> start_bit) & ((1 << count) - 1);
}

enum class replacement_policy {
    none /** */,
    lru /** Last recently used*/
};

template<bool thread_safe = false>
class cache {
private:
    std::mutex state_mutex_;
    std::string name_;
    uint cache_line_addressing_bit_;
    uint cache_global_addressing_bits_;
    enum replacement_policy policy_;
    uint n_assoc_;
    std::vector<size_t> table;
    size_t global_miss_counter = 0;
    size_t access_counter = 0;

    int access_data(size_t address_bytes, size_t read_bytes = 1) {
        if (n_assoc_ != 0) {
            abort();
        }
        int cache_lines_loaded = 0;
        for (size_t i = 0; i < read_bytes; ++i) {
            size_t byte_address = i + address_bytes;
            size_t num_line_in_cache = extract_bits(byte_address, cache_line_addressing_bit_, cache_global_addressing_bits_);
            size_t index = byte_address >> cache_line_addressing_bit_;
            if (index != table[num_line_in_cache]) {
                table[num_line_in_cache] = index;
                ++cache_lines_loaded;
            }
        }
        global_miss_counter += cache_lines_loaded;
        access_counter++;
        return cache_lines_loaded;
    }

public:
    cache(std::string name, uint line_size, uint size, enum replacement_policy policy = replacement_policy::none, uint n_assoc = 0) :
            name_(std::move(name)),
            cache_line_addressing_bit_(std::countr_zero(line_size)),
            cache_global_addressing_bits_(std::countr_zero(size / line_size)),
            policy_(policy),
            n_assoc_(n_assoc),
            table(size / line_size, -1) {
        assert(std::has_single_bit(line_size));
        assert(std::has_single_bit(size));
    };


    /**
     * Return whether an address was evicted from the cache
     * @param address
     * @param size bytes read
     * @return the number of cache misses caused by the read
     */

    template<typename T>
    int access_data(T *address) {
        if constexpr (thread_safe) {
            std::lock_guard m(state_mutex_);
            return access_data((size_t) ((void *) address), sizeof(T));
        } else {
            return access_data((size_t) ((void *) address), sizeof(T));
        }
    }

    [[nodiscard]] const std::string &getName() const {
        return name_;
    }

    /**
     * Cache line size in Bytes
     * @return
     */
    [[nodiscard]] size_t getCacheLineSize() const {
        return 1 << (cache_line_addressing_bit_);
    }


public:
    friend std::ostream &operator<<(std::ostream &os, const cache &cache) {
        os << " - Global cache report for: " << cache.name_ << ":\n";
        if (cache.access_counter) {
            os << "    - " << cache.name_ << " global_miss_counter: " << cache.global_miss_counter << " access_counter: " << cache.access_counter << " global_miss_rate: " <<
               100. * (double) cache.global_miss_counter / (double) cache.access_counter << " %\n";
        } else {
            os << "    - No data recorded\n";
        }

        return os;
    }
};

template<bool thread_safe>
struct cache_layer {
    std::shared_ptr<cache<thread_safe>> a_cache_;
    size_t cache_miss_counter_ = 0;
    size_t cache_access_counter_ = 0;
};
