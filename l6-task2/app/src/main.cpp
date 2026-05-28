#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zephyr/init.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "our_driver.h"



//#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
//#define LED_NODE DT_ALIAS(led0)

//#define LED_NODE DT_NODELABEL(green_led_2) //Another method to get the node identifier for the led_2 node

#define LED_NODE DT_PATH(leds, led_2) //Another method to get the node identifier for the led_2 node using the path in the devicetree

//#define LED_NODE DT_ALIAS(user_button) //Another method to get the node identifier for the user_button node using the alias defined in the devicetree

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);


LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

namespace {
    void test(uint32_t iteration_count) {
        const struct device *driver = DEVICE_DT_GET(DT_NODELABEL(our_driver));

        if (!device_is_ready(driver)) {
            LOG_ERR("our_driver not ready");
            return;
        }

        // Custom extension API call
        int res = our_driver_set_blink_count(driver, iteration_count);
        if (res == -EOVERFLOW) {
            LOG_WRN("blink_count overflowed, reset to 0");
        }

        // LED ON
        sensor_sample_fetch(driver);
        k_msleep(1000);

        struct sensor_value val;
        int ret = sensor_channel_get(driver, SENSOR_CHAN_AMBIENT_TEMP, &val);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
}

int main(void)
{
    
    bool led_state = true;

    if (!gpio_is_ready_dt(&led)) return 0;

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    while (1) {
        if (gpio_pin_toggle_dt(&led) < 0) return 0;


        led_state = !led_state;

        // l6-task2: Blink counter implementation
        static uint32_t i = 0U;
        if (i < UINT32_MAX) {
            i++;
        } else {
            i = 0U;
            LOG_WRN("Loop counter reset");
        }

        test(i);
        //LOG_INF("Hello World %s\n", CONFIG_BOARD);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS); 
    }
    return 0;
}