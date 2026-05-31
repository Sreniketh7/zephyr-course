#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <stdlib.h>
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

/* l7-task2: set — exposes our_driver_set_blink_count via shell */
static int cmd_set(const struct shell *sh, int argc, char **argv)
{
    /* argument count is enforced by SHELL_CMD_ARG(mandatory=2)
       but we validate the value range ourselves */

    char *endptr;
    long value = strtol(argv[1], &endptr, 10);

    /* check for non-numeric input */
    if (*endptr != '\0') {
        shell_error(sh, "invalid argument: '%s' is not a number", argv[1]);
        return -EINVAL;
    }

    /* check range */
    if (value < 0 || (uint32_t)value > OUR_DRIVER_BLINK_COUNT_MAX) {
        shell_error(sh, "value out of range: must be 0 to %lu", (unsigned long)OUR_DRIVER_BLINK_COUNT_MAX);
        return -EINVAL;
    }

    const struct device *dev = get_driver(sh);
    if (dev == NULL) {
        return -ENODEV;
    }

    int ret = our_driver_set_blink_count(dev, (uint32_t)value);
    if (ret == -EOVERFLOW) {
        shell_error(sh, "blink_count overflowed, reset to 0");
        return ret;
    }

    shell_print(sh, "blink_count set to %ld", value);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_subcmd,
    SHELL_CMD_ARG(fetch, NULL, "Call sensor_sample_fetch — turns LED ON",       cmd_fetch, 1, 0),
    SHELL_CMD_ARG(read,  NULL, "Call sensor_channel_get — turns LED OFF",       cmd_read,  1, 0),
    SHELL_CMD_ARG(info,  NULL, "Print device name and ready state",              cmd_info,  1, 0),
    SHELL_CMD_ARG(set,   NULL, "Set blink count <value> via custom API",         cmd_set,   2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_subcmd, "Commands for our_driver sensor", NULL);