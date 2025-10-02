#include "asm-generic/errno-base.h"
#include "linux/kstrtox.h"
#include "linux/stddef.h"
#include "linux/types.h"
#include "linux/uaccess.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/printk.h>

#include <linux/wait.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

#define MAJOR_NR 64

// threads
static struct task_struct *kthread_1;
static struct task_struct *kthread_2;

// watch variable to monitor the waitqueues
static long int watch_var = 0;

// static method to declare waitqueue - already initialized
DECLARE_WAIT_QUEUE_HEAD(wq1);

// dynamic method to declare waitqueue - has to be initialized
static wait_queue_head_t wq2;

int thread_function(void* wait_sel) {
  int selection = *(int * ) wait_sel;

  switch (selection) {
    case 1:
      /* 
       * wait_event()
       * check if watch_var is 11
       * - if not, go to sleep
       * - sleep until wake_up is called, then check again
      */
      wait_event(wq1, watch_var == 11);
      pr_info("wq - watch_var = 11\n");
      break;
    case 2:
      /*
       * wait_event_timeout()
       * if timeout elapsed AND the condition is NOT true --> returns 0
       * if timeout elapsed AND the condition is true --> returns 1
       * if the function is waked up via wake_up and condition is met --> returns the remaining time
      */
      while(wait_event_timeout(wq2, watch_var == 22, msecs_to_jiffies(5000)) == 0) {
        pr_info("wq - watch_var != 22, but timeout elapsed\n");
      }
      pr_info("wq - watch_var = 22\n");
      break;
    default:
      break;
  }
  pr_info("wq - thread monitoring wq%d finished exection!\n", selection);
  return 0;
}

static ssize_t write(struct file *f, const char *user_buf, size_t count, loff_t *offs) {
  int to_copy, not_copied, delta;
  char buffer[16];
  pr_info("wq - write callback called\n");

  memset(buffer, 0, sizeof(buffer));

  to_copy = min(count, sizeof(buffer));
  not_copied = copy_from_user(buffer, user_buf, to_copy);
  delta = to_copy - not_copied;

  // convert string to long int
  if (kstrtol(buffer, 10, &watch_var) == -EINVAL) {
    pr_info("wq - error converting input\n");
  } else {
    pr_info("wq - watch_var = %ld\n", watch_var);
  }

  wake_up(&wq1);
  wake_up(&wq2);

  return delta;
}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .write = write
};

static int __init wq_init(void) {
  static int t1 = 1;
  static int t2 = 2;
  printk("wq - init\n");

  // init dynamic wq
  init_waitqueue_head(&wq2);

  if (register_chrdev(MAJOR_NR, "dev_nr", &fops) != 0) {
    pr_info("wq - could not register device number\n");
    return -1;
  }
  pr_info("wq - device number registered\n");

  kthread_1 = kthread_run(thread_function, &t1, "kthread1");
  if (kthread_1 != NULL) {
    pr_info("wq - thread 1 running\n");
  } else {
    pr_info("wq - thread 1 could not be created\n");
    unregister_chrdev(MAJOR_NR, "dev_nr");
    return -1;
  } 

  kthread_2 = kthread_run(thread_function, &t2, "kthread2");
  if (kthread_2 != NULL) {
    pr_info("wq - thread 2 running\n");
  } else {
    pr_info("wq - thread 2 could not be created\n");
    unregister_chrdev(MAJOR_NR, "dev_nr");
    return -1;
  } 

  return 0;
}

static void __exit wq_exit(void) {
  printk("wq - exit\n");
  
  // make sure threads are stopped
  watch_var = 11;
  wake_up(&wq1);
  mdelay(10);

  watch_var = 22;
  wake_up(&wq2);
  mdelay(10);

  unregister_chrdev(MAJOR_NR, "dev_nr");
}

module_init(wq_init);
module_exit(wq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tomi");
MODULE_DESCRIPTION("Waitqueues and threads");
