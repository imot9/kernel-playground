#include <linux/module.h>
#include <linux/init.h>

static int __init hello_init(void) {
  printk("kernel_hello - Hello, Kernel!\n");
  return 0;
}

static void __exit hello_exit(void) {
  printk("kernel_hello - Goodbye, Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomi");
MODULE_DESCRIPTION("Kernel hello");
