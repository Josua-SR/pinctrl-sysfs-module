// SPDX-License-Identifier: (GPL-2.0+ OR BSD-3-Clause)
/*
 * Copyright 2023 Josua Mayer <josua@solid-run.com>
 */

#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/pinctrl/consumer.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>

static ssize_t state_show(struct kobject *kobj,
						  struct kobj_attribute *attr, char *buf)
{
	/* there is no function to get currently active pinctrl state */
	return -EOPNOTSUPP;
}

static ssize_t state_store(struct kobject *kobj,
								struct kobj_attribute *attr, char *buf,
								size_t count)
{
	struct device *dev = kobj_to_dev(kobj);
	struct pinctrl *pinctrl;
	char *eos;

	/* remove newline, if any */
	eos = strchrnul(buf, '\n');
	*eos = '\0';

	pinctrl = pinctrl_get_select(dev, buf);
	if (IS_ERR(pinctrl)) {
		dev_err(dev, "failed to select pinctrl state \"%s\": %d\n",
				buf, (int)PTR_ERR(pinctrl));
		return PTR_ERR(pinctrl);
	}
	dev_info(dev, "selected pinctrl state \"%s\"\n", buf);

    pinctrl_put(pinctrl);

	return count;
}

static struct kobj_attribute state_attribute =
	__ATTR(state, 0660, state_show, (void *)state_store);

int pinctrl_state_sysfs_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int ret;

	ret = sysfs_create_file(&dev->kobj, &state_attribute.attr);
	if (ret)
		dev_err(dev, "failed to create sysfs file: %d\n", ret);

	return ret;
}

int pinctrl_state_sysfs_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	sysfs_remove_file(&dev->kobj, &state_attribute.attr);

	return 0;
}

static const struct of_device_id pinctrl_state_sysfs_dt_ids[] = {
	{ .compatible = "pinctrl-state-sysfs", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, pinctrl_state_sysfs_dt_ids);

static struct platform_driver pinctrl_state_sysfs_driver = {
	.driver = {
		.name = "pinctrl-state-sysfs",
		.of_match_table	= of_match_ptr(pinctrl_state_sysfs_dt_ids),
	},
	.probe = pinctrl_state_sysfs_probe,
    .remove = pinctrl_state_sysfs_remove,
};
module_platform_driver(pinctrl_state_sysfs_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("pinctrl state sysfs control");
MODULE_AUTHOR("Josua Mayer <josua@solid-run.com>");
