#include "linux/sysfs.h"
#include "asm-generic/errno-base.h"
#include "linux/printk.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/string.h>

static ssize_t dummy_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
  return sprintf(buf, "sysfs - read called from /sys/kernel/%s/%s\n", kobj->name, attr->attr.name);
}

static ssize_t dummy_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
  pr_info("sysfs - write %s to /sys/kernel/%s/%s\n", buf, kobj->name, attr->attr.name);
  return count;
}

static struct kobject *dummy_kobj;
static struct kobj_attribute dummy_attr = __ATTR(dummy, 0660, dummy_show, dummy_store);

static int __init sf_init(void) {
  printk("sysfs - init\n");

  dummy_kobj = kobject_create_and_add("hello", kernel_kobj);
  if (dummy_kobj == NULL) {
    pr_info("sysfs - error creating /sys/kernel/hello\n");
    return -ENOMEM;
  }

  if (sysfs_create_file(dummy_kobj, &dummy_attr.attr)) {
    pr_info("sysfs - error creating /sys/kernel/hello/dummy\n");
    kobject_put(dummy_kobj);
    return -ENOMEM;
  }

  return 0;
}

static void __exit sf_exit(void) {
  printk("sysfs - exit\n");
  kobject_put(dummy_kobj);
}

module_init(sf_init);
module_exit(sf_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomi");
MODULE_DESCRIPTION("Sysfs");
