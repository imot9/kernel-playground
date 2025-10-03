#include "linux/device.h"
#include "linux/printk.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

// pdev --> information from device tree
static int dt_probe(struct platform_device *pdev);
static void dt_remove(struct platform_device *pdev);

// list of compatible devices
static struct of_device_id driver_ids[] = {
  {
    .compatible = "mydev",
  },
  { /* sentinel */ }
};

// assign compatible device list to the kernel module
MODULE_DEVICE_TABLE(of, driver_ids);

/*
 * .probe -> called upon registering the driver
 * .remove -> called upon removing the driver
*/
static struct platform_driver driver = {
  .probe = dt_probe,
  .remove = dt_remove,
  .driver = {
    .name = "device_driver",
    .of_match_table = driver_ids,
  },
};

static int dt_probe(struct platform_device *pdev) {
  struct device *dev = &pdev->dev;
  const char* label;
  int value, ret;

  if (!device_property_present(dev, "label")) {
    pr_info("dt - error, device property 'label' not present\n");
    return -1;
  }

  if (!device_property_present(dev, "value")) {
    pr_info("dt - error, device property 'value' not present\n");
    return -1;
  }

  ret = device_property_read_string(dev, "label", &label);
  if (ret) {
    pr_info("dt -  could not read 'label'\n");
    return -1;
  }
  pr_info("dt - label: %s\n", label);

  ret = device_property_read_u32(dev, "value", &value);
  if (ret) {
    pr_info("dt -  could not read 'value'\n");
    return -1;
  }
  pr_info("dt - value: %d\n", value);
  return 0;
}

static void dt_remove(struct platform_device *pdev) {
  pr_info("dt - remove\n");
}

static int __init dt_init(void) {
  printk("dt - init\n");

  if (platform_driver_register(&driver)) {
    pr_info("dt - could not load driver\n");
    return -1; 
  }

  return 0;
}

static void __exit dt_exit(void) {
  printk("dt - exit\n");
  platform_driver_unregister(&driver);
}

module_init(dt_init);
module_exit(dt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomi");
MODULE_DESCRIPTION("Devicetree");

