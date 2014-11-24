#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>


MODULE_LICENSE("GPL");

static int tstruct_init(void)
{
	printk(KERN_INFO "Initializing tstruct Module\n");
	struct task_struct *p,*q;
	p = &init_task;
	q = &init_task;
	do{
		p = list_entry(p->tasks.next, struct task_struct, tasks); // internally uses container_of
		printk(KERN_INFO "PID is %d, executable is %s \n",p->pid, p->comm);
		
	}while(p != q);
	return 0;
}

static void tstruct_exit(void)
{
	printk(KERN_INFO "Exiting tstruct Module\n");
}

module_init(tstruct_init);
module_exit(tstruct_exit);
