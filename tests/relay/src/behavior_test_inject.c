#define DT_DRV_COMPAT zmk_behavior_test_inject

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/split/output-relay/event.h>

LOG_MODULE_REGISTER(behavior_test_inject, CONFIG_ZMK_LOG_LEVEL);

// Declaration of the hidden injection function
extern int
zmk_split_peripheral_output_relay_test_inject_event(struct zmk_split_bt_output_relay_event *ev);

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
#include <zephyr/sys/printk.h>

    printk("Test inject behavior pressed\n");

    struct zmk_split_bt_output_relay_event ev = {
        .relay_channel = 0, // Target channel 0
        .value = 123,       // Test value
    };

    // Inject the event as if it came from BLE
    zmk_split_peripheral_output_relay_test_inject_event(&ev);

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_test_inject_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

static int behavior_test_inject_init(const struct device *dev) {
    printk("Inject behavior initialized\n");
    k_busy_wait(2000000);
    return 0;
}

static int sys_init_debug(void) {
    printk("SYS_INIT debug: behavior_test_inject.c is linked and running!\n");
    return 0;
}
SYS_INIT(sys_init_debug, APPLICATION, 90);

#define TEST_INJECT_INST(n)                                                                        \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_test_inject_init, NULL, NULL, NULL, POST_KERNEL,           \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                                   \
                            &behavior_test_inject_driver_api);

DT_INST_FOREACH_STATUS_OKAY(TEST_INJECT_INST)
