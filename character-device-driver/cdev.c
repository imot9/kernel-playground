#include "linux/printk.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

static int major;

static ssize_t c_read(struct file *f, char __user *u, size_t l, loff_t *o) {
  pr_info("cdev - Read called\n");
  return 0;
}

static int c_open(struct inode* inode, struct file* f) {
  pr_info("cdev - major: %d, minor: %d\n", imajor(inode), iminor(inode));
  pr_info("cdev - file_p->f_pos: %lld\n", f->f_pos);
  pr_info("cdev - file_p->f_mode: 0x%x\n", f->f_mode);
  pr_info("cdev - file_p->f_flags: 0x%x\n", f->f_flags);

  return 0;
}

static int c_release(struct inode* inode, struct file* f) {
  pr_info("cdev - File is closed\n");

  return 0;
}

static struct file_operations fops = {
  .read = c_read,
  .open = c_open,
  .release = c_release,
};

static int __init c_init(void) {
  // 0 - dynamic major device number allocation (return value is the devnum)
  // otherwise - retval is 0 (if successful), else error code
  major = register_chrdev(0, "cdev", &fops);

  if (major < 0) {
    pr_err("cdev - Error registering character device\n");
    return major;
  }

  pr_info("cdev - Major device number: %d\n", major);
  return 0;
}

static void __exit c_exit(void) {
  pr_info("cdev - Removing character device with number: %d\n", major);
  unregister_chrdev(major, "cdev");
}

module_init(c_init);
module_exit(c_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomi");
MODULE_DESCRIPTION("Driver for registering a character device");
