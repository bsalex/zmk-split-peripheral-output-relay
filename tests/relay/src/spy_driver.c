#define DT_DRV_COMPAT zmk_output_relay_spy

#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zmk/output/output_generic.h>

LOG_MODULE_REGISTER(spy_driver, CONFIG_ZMK_LOG_LEVEL);

static int spy_set_value(const struct device *dev, uint8_t value) {
    printk("Spy driver set_value called: %d\n", value);
    return 0;
}

static const struct output_generic_api spy_api = {
    .set_value = spy_set_value,
};

static int spy_init(const struct device *dev) { return 0; }

#define SPY_INST(n)                                                                                \
    DEVICE_DT_INST_DEFINE(n, spy_init, NULL, NULL, NULL, POST_KERNEL,                              \
                          CONFIG_APPLICATION_INIT_PRIORITY, &spy_api);

DT_INST_FOREACH_STATUS_OKAY(SPY_INST)
