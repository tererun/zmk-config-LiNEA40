#define DT_DRV_COMPAT tererun_input_processor_pointing_control

#include <zephyr/device.h>
#include <drivers/input_processor.h>

#include <linea40/pointing_control.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static int pointing_control_handle_event(const struct device *dev, struct input_event *event,
                                         uint32_t param1, uint32_t param2,
                                         struct zmk_input_processor_state *state) {
    return linea40_pointing_control_process(event) ? ZMK_INPUT_PROC_CONTINUE : ZMK_INPUT_PROC_STOP;
}

static const struct zmk_input_processor_driver_api pointing_control_driver_api = {
    .handle_event = pointing_control_handle_event,
};

#define POINTING_PROCESSOR_INST(n)                                                                \
    DEVICE_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                \
                          CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &pointing_control_driver_api);

DT_INST_FOREACH_STATUS_OKAY(POINTING_PROCESSOR_INST)

#endif
