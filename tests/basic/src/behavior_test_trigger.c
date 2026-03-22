#define DT_DRV_COMPAT zmk_behavior_test_trigger

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/split/output-relay/event.h>

LOG_MODULE_REGISTER(zmk_behavior_test_trigger, CONFIG_ZMK_LOG_LEVEL);

struct behavior_test_trigger_config {
    int relay_channel;
};

#ifdef CONFIG_ZMK_BEHAVIOR_TEST_TRIGGER_MOCK

int zmk_split_bt_invoke_output(const struct device *dev,
                               struct zmk_split_bt_output_relay_event event) {
    LOG_INF("Mock invoke output: channel %d value %d", event.relay_channel, event.value);
    return 0;
}

uint8_t relay_channel_get_for_device(const struct device *device) {
    LOG_DBG("Mock relay_channel_get_for_device");
    // Just return 1 for testing purposes if the device matches?
    // We don't have easy access to the config structure here to match.
    // But for the test we can just return 1.
    return 1;
}

#endif /* CONFIG_ZMK_BEHAVIOR_TEST_TRIGGER_MOCK */

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    const struct device *dev = device_get_binding(binding->behavior_dev);
    const struct behavior_test_trigger_config *cfg = dev->config;

    LOG_INF("Test trigger pressed: channel %d", cfg->relay_channel);

    struct zmk_split_bt_output_relay_event ev = {
        .value = 42,
    };

#ifdef CONFIG_ZMK_BEHAVIOR_TEST_TRIGGER_MOCK
    zmk_split_bt_invoke_output(NULL, ev);
#else
    // We try to find "test_relay" device. In the overlay we named it "test_relay".
    const struct device *target_dev = device_get_binding("test_relay");
    if (!target_dev) {
        LOG_ERR("Test device 'test_relay' not found");
        return ZMK_BEHAVIOR_OPAQUE;
    }

    zmk_split_bt_invoke_output(target_dev, ev);
#endif

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_test_trigger_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
};

#define TEST_TRIGGER_INST(n)                                                                       \
    static const struct behavior_test_trigger_config config_##n = {                                \
        .relay_channel = DT_PROP(DT_DRV_INST(n), relay_channel),                                   \
    };                                                                                             \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, &config_##n, POST_KERNEL,                         \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                                   \
                            &behavior_test_trigger_driver_api);

DT_INST_FOREACH_STATUS_OKAY(TEST_TRIGGER_INST)
