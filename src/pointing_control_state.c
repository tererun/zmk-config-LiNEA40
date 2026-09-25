#include <zephyr/kernel.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>

#include <linea40/pointing_control.h>

struct scale_ratio {
    int16_t multiplier;
    int16_t divisor;
};

static const struct scale_ratio mouse_scales[] = {
    {1, 2},
    {3, 4},
    {1, 1},
    {5, 4},
    {3, 2},
    {7, 4},
    {2, 1},
};

static const struct scale_ratio scroll_scales[] = {
    {1, 4},
    {1, 3},
    {7, 20},
    {2, 5},
    {1, 2},
    {1, 1},
    {2, 1},
    {3, 1},
};

static struct {
    struct k_spinlock lock;
    bool invert_scroll_x;
    bool invert_scroll_y;
    uint8_t mouse_scale;
    uint8_t scroll_scale;
    int16_t mouse_remainder_x;
    int16_t mouse_remainder_y;
    int16_t scroll_remainder_x;
    int16_t scroll_remainder_y;
} control_state = {
    .mouse_scale = 4,
    .scroll_scale = 2,
};

static void increment_level(uint8_t *level, size_t count) {
    if (*level + 1 < count) {
        (*level)++;
    }
}

static void decrement_level(uint8_t *level) {
    if (*level > 0) {
        (*level)--;
    }
}

void linea40_pointing_control_apply(enum linea40_pointing_control_action action) {
    k_spinlock_key_t key = k_spin_lock(&control_state.lock);

    switch (action) {
    case LINEA40_TOGGLE_SCROLL_Y:
        control_state.invert_scroll_y = !control_state.invert_scroll_y;
        break;
    case LINEA40_TOGGLE_SCROLL_X:
        control_state.invert_scroll_x = !control_state.invert_scroll_x;
        break;
    case LINEA40_MOUSE_SENSITIVITY_UP:
        increment_level(&control_state.mouse_scale, ARRAY_SIZE(mouse_scales));
        control_state.mouse_remainder_x = control_state.mouse_remainder_y = 0;
        break;
    case LINEA40_MOUSE_SENSITIVITY_DOWN:
        decrement_level(&control_state.mouse_scale);
        control_state.mouse_remainder_x = control_state.mouse_remainder_y = 0;
        break;
    case LINEA40_SCROLL_SPEED_UP:
        increment_level(&control_state.scroll_scale, ARRAY_SIZE(scroll_scales));
        control_state.scroll_remainder_x = control_state.scroll_remainder_y = 0;
        break;
    case LINEA40_SCROLL_SPEED_DOWN:
        decrement_level(&control_state.scroll_scale);
        control_state.scroll_remainder_x = control_state.scroll_remainder_y = 0;
        break;
    }

    k_spin_unlock(&control_state.lock, key);
}

static int16_t scale_value(int16_t value, const struct scale_ratio *ratio, int16_t *remainder) {
    int32_t numerator = (int32_t)value * ratio->multiplier + *remainder;
    int16_t scaled = numerator / ratio->divisor;
    *remainder = numerator - ((int32_t)scaled * ratio->divisor);
    return scaled;
}

bool linea40_pointing_control_process(struct input_event *event) {
    if (event->type != INPUT_EV_REL) {
        return true;
    }

    k_spinlock_key_t key = k_spin_lock(&control_state.lock);

    switch (event->code) {
    case INPUT_REL_X:
        event->value = scale_value(event->value, &mouse_scales[control_state.mouse_scale],
                                   &control_state.mouse_remainder_x);
        break;
    case INPUT_REL_Y:
        event->value = scale_value(event->value, &mouse_scales[control_state.mouse_scale],
                                   &control_state.mouse_remainder_y);
        break;
    case INPUT_REL_HWHEEL:
        event->value = scale_value(event->value, &scroll_scales[control_state.scroll_scale],
                                   &control_state.scroll_remainder_x);
        if (control_state.invert_scroll_x) {
            event->value = -event->value;
        }
        break;
    case INPUT_REL_WHEEL:
        event->value = scale_value(event->value, &scroll_scales[control_state.scroll_scale],
                                   &control_state.scroll_remainder_y);
        if (control_state.invert_scroll_y) {
            event->value = -event->value;
        }
        break;
    default:
        break;
    }

    bool should_continue = event->value != 0 || event->sync ||
                           (event->code != INPUT_REL_X && event->code != INPUT_REL_Y &&
                            event->code != INPUT_REL_HWHEEL && event->code != INPUT_REL_WHEEL);

    k_spin_unlock(&control_state.lock, key);
    return should_continue;
}
