#include "linux/printk.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "ioctl_test.h"

static int major;
int32_t answer = 42;

static int driver_open(struct inode* inode, struct file* f) {
  return 0;
}

static int driver_release(struct inode* inode, struct file* f) {
  return 0;
}

static long int ioctl(struct file *file, unsigned cmd, unsigned long arg) {
  struct my_struct test;
  switch (cmd) {
    case WR_VALUE:
      if (copy_from_user(&answer, (int32_t *) arg, sizeof(answer))) {
        pr_err("ioctl - Error copying data from user!\n");
      } else {
        pr_info("ioctl - Update the answer to %d\n", answer);
      }
      break;
    case RD_VALUE:
      if (copy_to_user((int32_t *) arg, &answer, sizeof(answer))) {
        pr_err("ioctl - Error copying data to user!\n");
      } else {
        pr_info("ioctl - The answer (%d) was copied!\n", answer);
      }
      break;
    case GREETER:
      if (copy_from_user(&test, (struct my_struct *) arg, sizeof(test))) {
        pr_err("ioctl - Error copying data from user!\n");
      } else {
        pr_info("ioctl - %d greets to %s", test.repeat, test.name);
      }
      break;
  }
  return 0;
}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = driver_open,
  .release = driver_release,
  .unlocked_ioctl = ioctl,
};

static int __init c_init(void) {
  major = register_chrdev(0, "ioctl", &fops);

  if (major < 0) {
    pr_err("ioctl - Error registering character device\n");
    return major;
  }

  pr_info("ioctl - Major device number: %d\n", major);
  return 0;
}

static void __exit c_exit(void) {
  pr_info("ioctl - Removing character device with number: %d\n", major);
  unregister_chrdev(major, "ioctl");
}

module_init(c_init);
module_exit(c_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomi");
MODULE_DESCRIPTION("IOCTL in Linux kernel module");

