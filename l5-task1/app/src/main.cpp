#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>



#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
//#define LED_NODE DT_ALIAS(led0)

//#define LED_NODE DT_NODELABEL(green_led_2) //Another method to get the node identifier for the led_2 node

#define LED_NODE DT_PATH(leds, led_2) //Another method to get the node identifier for the led_2 node using the path in the devicetree

//#define LED_NODE DT_ALIAS(user_button) //Another method to get the node identifier for the user_button node using the alias defined in the devicetree

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    bool led_state = true;

    if (!gpio_is_ready_dt(&led)) return 0;

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    while (1) {
        if (gpio_pin_toggle_dt(&led) < 0) return 0;

        led_state = !led_state;
        LOG_INF("Hello World %s\n", CONFIG_BOARD);
        k_msleep(SLEEP_TIME_MS);

        //printk("Hello World %s\n", CONFIG_BOARD);
    }
    return 0;
}