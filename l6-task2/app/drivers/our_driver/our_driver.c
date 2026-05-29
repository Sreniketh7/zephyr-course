#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include "our_driver.h"

#define DT_DRV_COMPAT our_driver

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

/* l6-task2: custom API implementation */
static int set_blink_count_impl(const struct device *dev, uint32_t count)
{
    if (dev == NULL) {
        return -EINVAL;
    }

    struct our_driver_data *data = (struct our_driver_data *)dev->data;

    if (data->blink_count == OUR_DRIVER_BLINK_COUNT_MAX) {
        LOG_WRN("blink_count at max, resetting to 0");
        data->blink_count = 0U;
        return -EOVERFLOW;
    }

    data->blink_count = count;
    printk("blink_count: %u\n", data->blink_count);
    return 0;
}

/* l6-task1: sample_fetch -> LED ON */
static int our_driver_sample_fetch(const struct device *dev,
                                    enum sensor_channel chan)
{
    struct our_driver_data *data = (struct our_driver_data *)dev->data;
    LOG_INF("sample_fetch: LED ON");
    return gpio_pin_set_dt(&data->led, 1);
}

/* l6-task1: channel_get -> LED OFF */
static int our_driver_channel_get(const struct device *dev,
                                   enum sensor_channel chan,
                                   struct sensor_value *val)
{
    struct our_driver_data *data = (struct our_driver_data *)dev->data;
    LOG_INF("channel_get: LED OFF");
    gpio_pin_set_dt(&data->led, 0);
    val->val1 = 0;
    val->val2 = 0;
    return 0;
}

static int our_driver_init(const struct device *dev)
{
    struct our_driver_data *data = (struct our_driver_data *)dev->data;
    data->led = (struct gpio_dt_spec)GPIO_DT_SPEC_INST_GET(0, gpios);
    data->blink_count = 0U;

    if (!gpio_is_ready_dt(&data->led)) {
        LOG_ERR("LED GPIO not ready");
        return -ENODEV;
    }

    gpio_pin_configure_dt(&data->led, GPIO_OUTPUT_INACTIVE);
    LOG_INF("our_driver initialized");
    return 0;
}

/* Unified vtable — sensor API embedded as first member */
static const struct our_driver_api our_api = {
    .sensor = {
        .sample_fetch = our_driver_sample_fetch,
        .channel_get  = our_driver_channel_get,
    },
    .set_blink_count = set_blink_count_impl,
};

static struct our_driver_data our_driver_data_0;

DEVICE_DT_INST_DEFINE(0, our_driver_init, NULL,
    &our_driver_data_0, NULL,
    POST_KERNEL, 80,
    &our_api);
