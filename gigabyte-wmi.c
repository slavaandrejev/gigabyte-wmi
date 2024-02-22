#include <linux/acpi.h>
#include <linux/device.h>
#include <linux/dmi.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Slava Andrejev");
MODULE_DESCRIPTION("Gigabyte WMI Support");

#define GB_GET_GUID "ABBC0F6F-8EA1-11d1-00A0-C90629100000"
#define GB_SET_GUID "ABBC0F75-8EA1-11d1-00A0-C90629100000"

MODULE_ALIAS("wmi:" GB_GET_GUID);
MODULE_ALIAS("wmi:" GB_SET_GUID);

static int dmi_check_cb(const struct dmi_system_id *dmi)
{
	pr_info("Identified laptop model '%s'\n", dmi->ident);

	return 1;
}

static const struct dmi_system_id gigabyte_supported_platforms[] __initconst = {
	{ .ident = "Aero 16 YE5",
	  .matches = { DMI_MATCH(DMI_SYS_VENDOR, "GIGABYTE"),
		       DMI_MATCH(DMI_PRODUCT_NAME, "AERO 16 YE5"),
		       DMI_MATCH(DMI_PRODUCT_SKU, "P86YE5") },
	  .callback = dmi_check_cb }
};

struct gigabyte_wmi {
	struct device *dev;
};
static struct gigabyte_wmi *gigabyte_wmi;

static ssize_t current_fan_step_store(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	return count;
}

static DEVICE_ATTR_WO(current_fan_step);

static struct attribute *fan_control_attrs[] = {
	&dev_attr_current_fan_step.attr,
	NULL,
};

static const struct attribute_group fan_control_attribute_group = {
	.name = "fan_control",
	.attrs = fan_control_attrs,
};

static int create_fan_control(struct platform_device *dev)
{
	return sysfs_create_group(&dev->dev.kobj, &fan_control_attribute_group);
}

static void remove_fan_control(struct platform_device *dev)
{
	sysfs_remove_group(&dev->dev.kobj, &fan_control_attribute_group);
}

static int gigabyte_wmi_probe(struct platform_device *pdev)
{
	platform_set_drvdata(pdev, gigabyte_wmi);
	gigabyte_wmi->dev = &pdev->dev;

	return 0;
}

static int gigabyte_wmi_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver gigabyte_wmi_driver = {.driver =
                                                         {
                                                             .name = "gigabyte-wmi",
                                                         },
                                                     .probe  = gigabyte_wmi_probe,
                                                     .remove = gigabyte_wmi_remove};

static int __init gigabyte_wmi_init(void)
{
	struct platform_device *pdev;
	int err;

	if (acpi_disabled) {
		return -ENODEV;
	}

	if (0 == dmi_check_system(gigabyte_supported_platforms)) {
		pr_err("This system is not supported by %s driver\n",
		       THIS_MODULE->name);
		return -ENODEV;
	}

	if (!wmi_has_guid(GB_GET_GUID)) {
		pr_err("%s: No get methods found in WMI ACPI\n",
		       THIS_MODULE->name);
		return -ENODEV;
	}

	if (!wmi_has_guid(GB_SET_GUID)) {
		pr_err("%s: No set methods found in WMI ACPI\n",
		       THIS_MODULE->name);
		return -ENODEV;
	}

	gigabyte_wmi = kzalloc(sizeof(struct gigabyte_wmi), GFP_KERNEL);
	if (!gigabyte_wmi) {
		return -ENOMEM;
	}

	err = platform_driver_register(&gigabyte_wmi_driver);
	if (err) {
		goto pdrv_err;
	}

	pdev = platform_device_register_simple("gigabyte-wmi",
					       PLATFORM_DEVID_NONE, NULL, 0);
	if (IS_ERR(pdev)) {
		err = PTR_ERR(pdev);
		goto pdev_err;
	}

	err = create_fan_control(pdev);
	if (err) {
		goto create_fan_control_err;
	}

	return 0;

create_fan_control_err:
	platform_device_unregister(pdev);
pdev_err:
	platform_driver_unregister(&gigabyte_wmi_driver);
pdrv_err:
	kfree(gigabyte_wmi);
	return err;
}

static void __exit gigabyte_wmi_cleanup(void)
{
	struct platform_device *pdev = to_platform_device(gigabyte_wmi->dev);
	remove_fan_control(pdev);
	platform_device_unregister(pdev);
	platform_driver_unregister(&gigabyte_wmi_driver);
	kfree(gigabyte_wmi);
}

module_init(gigabyte_wmi_init);
module_exit(gigabyte_wmi_cleanup);
