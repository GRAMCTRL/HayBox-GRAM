/* Run from the repository root:
g++ -std=c++17 -Wall -Wextra -Werror -I test/debounce/stubs -I HAL/pico/include \
    test/debounce/debounce_test.cpp -o /tmp/haybox-debounce-test
/tmp/haybox-debounce-test
*/

#include "input/debounce.hpp"

#include <cstdlib>
#include <iostream>

void check(
    DebounceState &state,
    uint64_t time_us,
    bool reading,
    bool expected_pressed,
    bool expected_changed,
    uint32_t period_ms = 5
) {
    test_time_us = time_us;
    bool changed = update_debounce_state(state, reading, period_ms);
    if (state.pressed != expected_pressed || changed != expected_changed) {
        std::cerr << "At " << time_us << " us, reading=" << reading
                  << ": expected pressed=" << expected_pressed << ", changed=" << expected_changed
                  << "; got pressed=" << state.pressed << ", changed=" << changed << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void test_press_with_contact_dropout() {
    DebounceState state;
    check(state, 0, false, false, false);
    check(state, 1000, true, true, true);
    // A brief dropout 8 ms into a held press must not create another press.
    check(state, 9000, false, true, false);
    check(state, 9500, true, true, false);
    check(state, 14000, true, true, false);
    check(state, 100000, false, true, false);
    check(state, 104999, false, true, false);
    check(state, 105000, false, false, true);
    check(state, 106000, false, false, false);
}

void test_release_must_be_continuously_stable() {
    DebounceState state;
    check(state, 0, true, true, true);
    check(state, 100000, false, true, false);
    check(state, 104999, true, true, false);
    check(state, 105000, false, true, false);
    check(state, 109999, false, true, false);
    check(state, 110000, false, false, true);
    // A fresh press is accepted immediately after a confirmed release.
    check(state, 110001, true, true, true);
}

void test_short_tap() {
    DebounceState state;
    check(state, 0, true, true, true);
    check(state, 1000, false, true, false);
    check(state, 5999, false, true, false);
    check(state, 6000, false, false, true);
}

void test_contact_closes_at_release_deadline() {
    DebounceState state;
    check(state, 0, true, true, true);
    check(state, 1000, false, true, false);
    check(state, 6000, true, true, false);
    check(state, 7000, false, true, false);
    check(state, 11999, false, true, false);
    check(state, 12000, false, false, true);
}

void test_zero_debounce() {
    DebounceState state;
    check(state, 0, false, false, false, 0);
    check(state, 1, true, true, true, 0);
    check(state, 2, false, false, true, 0);
    check(state, 3, true, true, true, 0);
}

void test_independent_buttons_and_custom_period() {
    DebounceState first;
    DebounceState second;
    check(first, 0, true, true, true, 10);
    check(first, 1000, false, true, false, 10);
    check(second, 2000, true, true, true);
    check(second, 3000, false, true, false);
    check(second, 8000, false, false, true);
    check(first, 10999, false, true, false, 10);
    check(first, 11000, false, false, true, 10);
}

void test_long_hold_and_large_timestamp() {
    DebounceState state;
    check(state, 0, true, true, true);
    check(state, 4294967000ULL, true, true, false);
    check(state, 4294967100ULL, false, true, false);
    check(state, 4294972099ULL, false, true, false);
    check(state, 4294972100ULL, false, false, true);
}

int main() {
    test_press_with_contact_dropout();
    test_release_must_be_continuously_stable();
    test_short_tap();
    test_contact_closes_at_release_deadline();
    test_zero_debounce();
    test_independent_buttons_and_custom_period();
    test_long_hold_and_large_timestamp();
    std::cout << "All debounce tests passed\n";
}
