#ifndef _DEBOUNCE_HPP
#define _DEBOUNCE_HPP

#include "stdlib.hpp"

typedef struct _DebounceState {
    absolute_time_t release_at = 0;
    bool release_pending = false;
    bool pressed = false;
} DebounceState;

inline bool update_debounce_state(
    DebounceState &debounce_state,
    bool current_reading,
    uint32_t debounce_period_ms
) {
    // Accept presses immediately. Any closed-contact reading cancels a pending release,
    // including when the contact briefly opens while the button is being pressed.
    if (current_reading) {
        debounce_state.release_pending = false;
        bool state_changed = !debounce_state.pressed;
        debounce_state.pressed = true;
        return state_changed;
    }

    if (!debounce_state.pressed) {
        return false;
    }

    // Report a release only after the contact has remained open for the whole period.
    // This delays releases by debounce_period_ms and filters shorter release pulses.
    if (!debounce_state.release_pending) {
        debounce_state.release_pending = true;
        debounce_state.release_at = make_timeout_time_ms(debounce_period_ms);
    }
    if (!time_reached(debounce_state.release_at)) {
        return false;
    }

    debounce_state.pressed = false;
    debounce_state.release_pending = false;
    return true;
}

#endif
