#include "linux/printk.h"
#include "linux/uaccess.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

static int major;
static char text[64];

static ssize_t c_read(struct file *f, char __user *user_buf, size_t l, loff_t *off) {
  int not_copied, delta;
  // how many bytes to copy based on the text[] buf size
  int to_copy = (l + *off) < sizeof(text) ? l : (sizeof(text) - *off);

  pr_info("cdev - Read called\n");
  pr_info("cdev - Want to read: %ld bytes, only copying %d bytes. Offset: %lld\n", l, to_copy, *off);
  
  // cant read out more byes if we reached the text buf end
  if (*off >= sizeof(text)) {
    return 0;
  }
  
  // returns amount of bytes not copied
  not_copied = copy_to_user(user_buf, &text[*off], to_copy);
  delta = to_copy - not_copied;

  if (not_copied) {
    pr_warn("cdev - Could only copy %d bytes\n", delta);
  }
  
  // increase offset
  *off += delta;

  return delta;
}

static ssize_t c_write(struct file *f, const char __user *user_buf, size_t l, loff_t *off) {
  int not_copied, delta;
  int to_copy = (l + *off) < sizeof(text) ? l : (sizeof(text) - *off);

  pr_info("cdev - Write called\n");
  pr_info("cdev - Want to write: %ld bytes, only copying %d bytes. Offset: %lld\n", l, to_copy, *off);
  
  if (*off >= sizeof(text)) {
    return 0;
  }
  
  not_copied = copy_from_user(&text[*off], user_buf, to_copy);
  delta = to_copy - not_copied;

  if (not_copied) {
    pr_warn("cdev - Could only copy %d bytes\n", delta);
  }

  *off += delta;

  return delta;
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
  .write = c_write,
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
