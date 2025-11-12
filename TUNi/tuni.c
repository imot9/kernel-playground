#include "linux/device/class.h"
#include "linux/fs.h"
#include "linux/cdev.h"
#include "linux/printk.h"
#include "linux/module.h"
#include "linux/init.h"
#include "linux/device.h"

int tuni_open(struct inode *, struct file *);
int tuni_release(struct inode *, struct file *);

static struct class *tuni;
static struct cdev tuni_cdev;
static dev_t tuni_devt;
static struct file_operations tuni_fops = {
  .owner = THIS_MODULE,
  .open = tuni_open,
  .release = tuni_release,
};

static int __init tuni_init(void) {
  tuni = class_create("TUNi");
  
  // Register character device (/dev/tuni0)
  alloc_chrdev_region(&tuni_devt, 0, 1, "TUNi");
  cdev_init(&tuni_cdev, &tuni_fops);
  cdev_add(&tuni_cdev, tuni_devt, 1);

  // Add sysfs entry for TUNi (/sys/class/TUNi/tuni0)
  struct device *dev_tuni = device_create(tuni, NULL, tuni_devt, NULL, "tuni0");

  if (IS_ERR(dev_tuni)) {
			pr_err("Failed to create device. Aborting.\n" );

      device_destroy(tuni, tuni_devt);
      cdev_del(&tuni_cdev);
      unregister_chrdev_region(tuni_devt, 1);
      class_destroy(tuni);
			return -ENODEV;
  }

  pr_info("TUNi - module init\n");
  return 0;
}

static void __exit tuni_exit(void) {
  device_destroy(tuni, tuni_devt);
  cdev_del(&tuni_cdev);
  unregister_chrdev_region(tuni_devt, 1);
  class_destroy(tuni);

  printk("TUNi - module exit\n");
}

int tuni_open(struct inode *inode, struct file *f) {
  pr_info("tuni - open");
  return 0;
}
int tuni_release(struct inode *inode, struct file *f) {
  pr_info("tuni - release");
  return 0;
}

module_init(tuni_init);
module_exit(tuni_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomi");
MODULE_DESCRIPTION("Tiny Userspace NIC");

