#include "linux/hrtimer_types.h"
#include "linux/ktime.h"
#include "linux/time.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>

static struct hrtimer hr_timer;
u64 start_t;

static enum hrtimer_restart test_hrtimer_handler(struct hrtimer *timer) {
  // Get current time
  u64 now_t = jiffies;
  printk("start_t - now_t = %u\n", jiffies_to_msecs(now_t - start_t));
  return HRTIMER_NORESTART;
}

static int __init mod_init(void) {
  pr_info("hr_timer - Hello, kernel");

  // Init hrtimer
  hrtimer_setup(&hr_timer, &test_hrtimer_handler, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

  start_t = jiffies;
  hrtimer_start(&hr_timer, ms_to_ktime(100), HRTIMER_MODE_REL);
  return 0;
}

static void __exit mod_exit(void) {
  hrtimer_cancel(&hr_timer);
  pr_info("hr_timer - Goodbye, kernel");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomi");
MODULE_DESCRIPTION("A simple LKM using High Resolution Timers");
