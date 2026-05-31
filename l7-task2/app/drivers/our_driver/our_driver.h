#ifndef OUR_DRIVER_H
#define OUR_DRIVER_H

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <stdint.h>

#define OUR_DRIVER_BLINK_COUNT_MAX UINT32_MAX

/* Dynamic data struct */
struct our_driver_data {
    struct gpio_dt_spec led;
    volatile uint32_t blink_count;
};

/* Custom API vtable — sensor_driver_api MUST be first member */
struct our_driver_api {
    struct sensor_driver_api sensor;
    int (*set_blink_count)(const struct device *dev, uint32_t count);
};

/* Dispatch macro */
static inline int our_driver_set_blink_count(const struct device *dev, uint32_t count)
{
    const struct our_driver_api *api = (const struct our_driver_api *)dev->api;
    return api->set_blink_count(dev, count);
}

#endif /* OUR_DRIVER_H */