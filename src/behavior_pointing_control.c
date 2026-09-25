#define DT_DRV_COMPAT tererun_behavior_pointing_control

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zmk/behavior.h>

#include <linea40/pointing_control.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int pointing_control_pressed(struct zmk_behavior_binding *binding,
                                    struct zmk_behavior_binding_event event) {
    linea40_pointing_control_apply((enum linea40_pointing_control_action)binding->param1);
    return ZMK_BEHAVIOR_OPAQUE;
}

static int pointing_control_released(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api pointing_control_driver_api = {
    .binding_pressed = pointing_control_pressed,
    .binding_released = pointing_control_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

#define POINTING_CONTROL_INST(n)                                                                  \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                               \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &pointing_control_driver_api);

DT_INST_FOREACH_STATUS_OKAY(POINTING_CONTROL_INST)

#endif
