#include "linux/kern_levels.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

static int major;

static ssize_t c_read(struct file *f, char __user *u, size_t l, loff_t *o) {
  printk(KERN_DEBUG "cdev - Read called\n");
  return 0;
}

static struct file_operations fops = {
  .read = c_read
};

static int __init c_init(void) {
  // 0 - dynamic major device number allocation (return value is the devnum)
  // otherwise - retval is 0 (if successful), else error code
  major = register_chrdev(0, "cdev", &fops);

  if (major < 0) {
    printk(KERN_ERR "cdev - Error registering character device\n");
    return major;
  }

  printk(KERN_INFO "cdev - Major device number: %d\n", major);
  return 0;
}

static void __exit c_exit(void) {
  printk(KERN_INFO "cdev - Removing character device with number: %d\n", major);
  unregister_chrdev(major, "cdev");
}

module_init(c_init);
module_exit(c_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomi");
MODULE_DESCRIPTION("Driver for registering a character device");
