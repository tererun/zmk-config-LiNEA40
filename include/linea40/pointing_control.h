#pragma once

#include <zephyr/input/input.h>

enum linea40_pointing_control_action {
    LINEA40_TOGGLE_SCROLL_Y,
    LINEA40_TOGGLE_SCROLL_X,
    LINEA40_MOUSE_SENSITIVITY_UP,
    LINEA40_MOUSE_SENSITIVITY_DOWN,
    LINEA40_SCROLL_SPEED_UP,
    LINEA40_SCROLL_SPEED_DOWN,
};

void linea40_pointing_control_apply(enum linea40_pointing_control_action action);
bool linea40_pointing_control_process(struct input_event *event);
