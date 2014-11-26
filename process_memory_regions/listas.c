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

#define MYPID 1952 /* enter a valid PID here */

static int listas_init(void)
{
	printk(KERN_INFO "Initializing Listas Module\n");
	struct task_struct *p;
	struct vm_area_struct *vmap;
	p = &init_task;
	do{
		p = list_entry(p->tasks.next, struct task_struct, tasks);
	}while(p->pid != MYPID);
	printk(KERN_INFO "Beginning of the code segment = 0x%x, End of the code segment = 0x%x\n", p->mm->start_code,p->mm->end_code);
	printk(KERN_INFO "Beginning of the data segment = 0x%x, End of the data segment = 0x%x\n", p->mm->start_data,p->mm->end_data);
	printk(KERN_INFO "Beginning of the heap segment = 0x%x, Size of the heap segment = 0x%x\n", p->mm->start_brk,p->mm->brk);
	printk(KERN_INFO "Beginning of the stack segment = 0x%x\n", p->mm->start_stack);
	printk(KERN_INFO "vm_start = 0x%x, vm_end = 0x%x, size = %d\n", p->mm->mmap->vm_start, p->mm->mmap->vm_end, p->mm->mmap->vm_end - p->mm->mmap->vm_start);

	vmap = p->mm->mmap;
	printk(KERN_INFO "vm_start = 0x%x, vm_end = 0x%x, size = %d\n", vmap->vm_start, vmap->vm_end, vmap->vm_end - vmap->vm_start);
	while((vmap = vmap->vm_next) != NULL){
		printk(KERN_INFO "vm_start = 0x%x, vm_end = 0x%x, size = %d\n", vmap->vm_start, vmap->vm_end, vmap->vm_end - vmap->vm_start);
	}	
	return 0;
}

static void listas_exit(void)
{
	printk(KERN_INFO "Exiting Listas Module\n");
}

module_init(listas_init);
module_exit(listas_exit);
