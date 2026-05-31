#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "our_driver.h"

static const struct device *get_driver(const struct shell *sh)
{
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(our_driver));
    if (!device_is_ready(dev)) {
        shell_error(sh, "our_driver not ready");
        return NULL;
    }
    return dev;
}

/* fetch — calls sensor_sample_fetch() -> LED ON */
static int cmd_fetch(const struct shell *sh, int argc, char **argv)
{
    const struct device *dev = get_driver(sh);
    if (dev == NULL) {
        return -ENODEV;
    }

    int ret = sensor_sample_fetch(dev);
    if (ret < 0) {
        shell_error(sh, "sensor_sample_fetch failed: %d", ret);
        return ret;
    }

    shell_print(sh, "fetch OK — LED ON");
    return 0;
}

/* read — calls sensor_channel_get() -> LED OFF + prints result */
static int cmd_read(const struct shell *sh, int argc, char **argv)
{
    const struct device *dev = get_driver(sh);
    if (dev == NULL) {
        return -ENODEV;
    }

    struct sensor_value val;
    int ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &val);
    if (ret < 0) {
        shell_error(sh, "sensor_channel_get failed: %d", ret);
        return ret;
    }

    shell_print(sh, "read OK — LED OFF");
    shell_print(sh, "val1: %d  val2: %d", val.val1, val.val2);
    return 0;
}

/* info — prints device name and ready state */
static int cmd_info(const struct shell *sh, int argc, char **argv)
{
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(our_driver));

    shell_print(sh, "device name  : %s", dev->name);
    shell_print(sh, "device ready : %s", device_is_ready(dev) ? "yes" : "no");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_subcmd,
    SHELL_CMD_ARG(fetch, NULL, "Call sensor_sample_fetch — turns LED ON",  cmd_fetch, 1, 0),
    SHELL_CMD_ARG(read,  NULL, "Call sensor_channel_get — turns LED OFF",  cmd_read,  1, 0),
    SHELL_CMD_ARG(info,  NULL, "Print device name and ready state",        cmd_info,  1, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_subcmd, "Commands for our_driver sensor", NULL);