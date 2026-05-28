#ifndef OUR_DRIVER_H
#define OUR_DRIVER_H

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>

#define OUR_DRIVER_BLINK_COUNT_MAX UINT32_MAX

struct our_driver_data {
    struct gpio_dt_spec led;
    volatile uint32_t blink_count;
};

static inline int our_driver_set_blink_count(const struct device *dev, uint32_t count)
{
    if (dev == NULL) {
        return -EINVAL;
    }

    struct our_driver_data *data = (struct our_driver_data *)dev->data;

    if (data->blink_count == OUR_DRIVER_BLINK_COUNT_MAX) {
        data->blink_count = 0U;
        return -EOVERFLOW;
    }

    data->blink_count = count;
    printk("blink_count: %u\n", data->blink_count);
    return 0;
}

#endif /* OUR_DRIVER_H */