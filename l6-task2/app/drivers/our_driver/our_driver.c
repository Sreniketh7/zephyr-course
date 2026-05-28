#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include "our_driver.h"

#define DT_DRV_COMPAT our_driver

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

/* sample_fetch -> LED ON */
static int our_driver_sample_fetch(const struct device *dev,
                                    enum sensor_channel chan)
{
    struct our_driver_data *data = (struct our_driver_data *)dev->data;
    LOG_INF("sample_fetch: LED ON");
    return gpio_pin_set_dt(&data->led, 1);
}

/* channel_get -> LED OFF */
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

static const struct sensor_driver_api our_driver_api = {
    .sample_fetch = our_driver_sample_fetch,
    .channel_get  = our_driver_channel_get,
};

static struct our_driver_data our_driver_data_0;

DEVICE_DT_INST_DEFINE(0, our_driver_init, NULL,
    &our_driver_data_0, NULL,
    POST_KERNEL, 80,
    &our_driver_api);