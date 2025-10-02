#include "asm-generic/errno-base.h"
#include "linux/printk.h"
#include "linux/types.h"
#include "linux/uaccess.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

// procfs folder and file
static struct proc_dir_entry *proc_folder;
static struct proc_dir_entry *proc_file;

static ssize_t read(struct file *f, char *user_buf, size_t count, loff_t *offs) {
  char text[] = "hello from procfs!\n";
  int to_copy, not_copied, delta;

  to_copy = min(count, sizeof(text));
  not_copied = copy_to_user(user_buf, text, to_copy);
  delta = to_copy - not_copied;

  return delta;
}

static ssize_t write(struct file *f, const char *user_buf, size_t count, loff_t *offs) {
  char text[255];
  int to_copy, not_copied, delta;

  memset(text, 0, sizeof(text));
  to_copy = min(count, sizeof(text));
  not_copied = copy_from_user(text, user_buf, to_copy);
  delta = to_copy - not_copied;

  pr_info("procfs - %s\n", text);

  return delta;
}

static struct proc_ops fops = {
  .proc_read = read,
  .proc_write = write,
};

static int __init procfs_init(void) {
  printk("procfs - init\n");

  proc_folder = proc_mkdir("hello", NULL);
  if (proc_folder == NULL) {
    pr_info("procfs - error creating /proc/hello\n");
    return -ENOMEM;
  }

  proc_file = proc_create("dummy", 0666, proc_folder, &fops);
  if (proc_file == NULL) {
    pr_info("procfs - error creating dummy\n");
    proc_remove(proc_folder);
    return -ENOMEM;
  }

  pr_info("procfs - created /proc/hello/dummy\n");
  return 0;
}

static void __exit procfs_exit(void) {
  printk("procfs - exit\n");
  proc_remove(proc_file);
  proc_remove(proc_folder);
}

module_init(procfs_init);
module_exit(procfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomi");
MODULE_DESCRIPTION("Procfs");
