#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "our_driver.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    const struct device *driver = DEVICE_DT_GET(DT_NODELABEL(our_driver));

    if (!device_is_ready(driver)) {
        LOG_ERR("our_driver not ready");
        return -ENODEV;
    }

    LOG_INF("our_driver ready — use shell commands:");
    LOG_INF("  sensor set <value>");
    LOG_INF("  sensor fetch");
    LOG_INF("  sensor read");
    LOG_INF("  sensor info");

    while (1) {
        k_msleep(1000);
    }

    return 0;
}