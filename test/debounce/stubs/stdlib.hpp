#ifndef TEST_DEBOUNCE_STDLIB_HPP
#define TEST_DEBOUNCE_STDLIB_HPP

#include <cstdint>

using absolute_time_t = uint64_t;
inline uint64_t test_time_us = 0;

inline bool time_reached(absolute_time_t deadline) {
    return test_time_us >= deadline;
}

inline absolute_time_t make_timeout_time_ms(uint32_t milliseconds) {
    return test_time_us + uint64_t(milliseconds) * 1000;
}

#endif
