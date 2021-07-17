#pragma once

#include <string>
#include <utility>

#include <cmath>
#include <ostream>

template<bool thread_safe>
class profiler;

/**
 * Tracks the usage of a branch. Non copyable/movable/assignable.
 * @tparam thread_safe
 */
template<bool thread_safe>
class branch_profiler {
private:
    std::mutex state_mutex_;
    std::string name_;
    profiler<thread_safe> *parent_;
    size_t true_counter_ = 0 /** Counts */;
    size_t counter_ = 0;

private:

    [[nodiscard]] double compute_shannon_entropy() const {
        double p = (double) true_counter_ / (double) counter_;
        return -p * std::log(p) - (1 - p) * log(1 - p);
    }

public:
    explicit branch_profiler(std::string name, profiler<thread_safe> *parent) : name_(std::move(name)), parent_(parent) {
        parent_->register_branch_profiler(this);
    }

    branch_profiler(const branch_profiler &) = delete;

    branch_profiler &operator=(const branch_profiler) = delete;

    branch_profiler &operator=(branch_profiler &&other) = delete;

    virtual ~branch_profiler() {
        parent_->unregister_branch_profiler(this);
    }

    bool operator()(bool b) {
        if constexpr(thread_safe) {
            std::lock_guard m(state_mutex_);
            true_counter_ += b;
            counter_++;
        } else {
            true_counter_ += b;
            counter_++;
        }
        return b;
    }


    friend std::ostream &operator<<(std::ostream &os, const branch_profiler &profiler) {
        os << " - Branching report for " << profiler.name_ << ":\n";
        if (!profiler.counter_) {
            os << "No branching recorded \n";
        } else {
            os << "    - counter: " << profiler.counter_ << " branches_taken: " << profiler.true_counter_ << " taking_probability: " << (double) profiler.true_counter_ / (double) profiler.counter_
               << '\n';
            os << "    - shannon_entropy: " << profiler.compute_shannon_entropy() << '\n';
        }
        return os;
    }

};


