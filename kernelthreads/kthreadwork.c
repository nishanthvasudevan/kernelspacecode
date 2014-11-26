#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>


MODULE_LICENSE("GPL");

#define MESSAGE_LOG_LIMIT      50

/* This means 2 writer and 2 reader */
#define NUM_THREAD_PAIRS       5

#define EXIT_THREAD            0xFFFF
#define WAKE_CONDITION         ( ( sequencer == thread_id ) || ( exit_marker == EXIT_THREAD ) )

/* not macro to avoid mult operation at each usage */
static unsigned int NUM_THREADS = 2 * NUM_THREAD_PAIRS;
struct task_struct** thread_ids;

static DECLARE_WAIT_QUEUE_HEAD(resource_access_q);
static unsigned int sequencer = 0;
static unsigned int exit_marker = 0;

typedef struct crit_resource_s
{
        unsigned char data[ 64 ];
}crit_resource_str;
crit_resource_str*   crit_resource;

static void module_cleanup( void )
{
        unsigned int i = 0;

        /* any thread stuck in wait queue will be bumped off through this */
        exit_marker = EXIT_THREAD;
        wake_up_interruptible(&resource_access_q);

        /* this loop will exit only when all threads stop */
        for( i = 0; i < NUM_THREADS; i++ )
        {
                if( thread_ids[ i ] )
                {
                        kthread_stop( thread_ids[ i ] );
                }
        }

        /* free all the memory allocated by module */
        kfree( thread_ids );
        kfree( crit_resource );
}


static int reader_thread( void* thread_data )
{
        unsigned int thread_id = ( unsigned int )thread_data;
        unsigned int loop_counter = 0;
        unsigned char read_data[ 64 ];

        printk( KERN_INFO "kthreadwork: %s created\n", current->comm );

        while ( !kthread_should_stop() )
        {
                loop_counter++;

                /* wait till turn to read */
                wait_event_interruptible(resource_access_q, WAKE_CONDITION  );
                if( sequencer == thread_id )
                {
                        /* just like that ..:-) */
                        sprintf( read_data, "%s", crit_resource->data );

                        /* throttle to avoid disk overrun */
                        if( loop_counter < MESSAGE_LOG_LIMIT )
                                printk( KERN_INFO "%s consumed %s\n", current->comm, read_data );
                        else if( loop_counter == MESSAGE_LOG_LIMIT )
                                printk( KERN_INFO "%s continues to work but logging stopped to avoid disk overrun\n", current->comm);

                        /* kick-off next thread */
                        sequencer = (sequencer + 1) % NUM_THREADS;
                        wake_up_interruptible(&resource_access_q);
                }
                else if( exit_marker == EXIT_THREAD )
                {
                        break;
                }
        }

        return 0;
}

static int writer_thread( void* thread_data )
{
        unsigned int thread_id = ( unsigned int )thread_data;
        unsigned int seq_start = thread_id / 2;
        unsigned int current_seq = seq_start;
        unsigned int loop_counter = 0;

        printk( KERN_INFO "kthreadwork: %s created seq_start: %d\n", current->comm, seq_start );

        while ( !kthread_should_stop() )
        {
                loop_counter++;

                /* insert into wait q until turn to write*/
                wait_event_interruptible(resource_access_q, WAKE_CONDITION );
                if( sequencer == thread_id )
                {
                        /* generate work */
                        sprintf( crit_resource->data, "%s %d", "Work Seq Num", current_seq );

                        /* throttle to avoid disk overrun */
                        if( loop_counter < MESSAGE_LOG_LIMIT )
                                printk( KERN_INFO "%s generated %s\n", current->comm, crit_resource->data );
                        else if( loop_counter == MESSAGE_LOG_LIMIT )
                                printk( KERN_INFO "%s continues to work but logging stopped to avoid disk overrun\n", current->comm);

                        current_seq += NUM_THREAD_PAIRS;

                        /* kick-off next thread */
                        sequencer = (sequencer + 1) % NUM_THREADS;
                        wake_up_interruptible(&resource_access_q);
                }
                else if( exit_marker == EXIT_THREAD )
                {
                        break;
                }
        }

        return 0;
}

static unsigned int create_threads( void )
{
        unsigned int i = 0;
        unsigned int index = 0;

        for( i = 0; i < NUM_THREAD_PAIRS; i++ )
        {
                index = 2*i;
                thread_ids[ index ] = kthread_run( writer_thread, ( void* )( index ), "%s-%d", "writer", i );
                if ( IS_ERR( thread_ids[ index ] ) )
                {
                        printk(KERN_INFO "kthreadwork: writer_thread: %d creation failed\n", i );
                        thread_ids[ index ] = 0;
                        return 1;
                }

                index = ( 2*i ) + 1;
                thread_ids[ index ] = kthread_run( reader_thread, ( void* )( index ), "%s-%d", "reader", i );
                if ( IS_ERR( thread_ids[ index ] ) )
                {
                        printk(KERN_INFO "kthreadwork: reader_thread: %d creation failed\n", i );
                        thread_ids[ index ] = 0;
                        return 1;
                }
        }

        return 0;
}


static int kthreadwork_init(void)
{
	printk(KERN_INFO "Initializing kthreadwork module\n");

        thread_ids = kzalloc( sizeof( struct task_struct*) * NUM_THREADS, GFP_KERNEL );

        if( !thread_ids )
        {
                printk(KERN_INFO "No memory for thread_ids in kthreadwork module\n");
                return 1;
        }

        crit_resource = kzalloc( sizeof( crit_resource_str ), GFP_KERNEL );

        if( !crit_resource )
        {
                printk(KERN_INFO "No memory for crit_resource_ptr in kthreadwork module\n");
                kfree( thread_ids );
                return 2;
        }

        if( ( create_threads( ) ) )
        {
                module_cleanup( );
                return 3;
        }

	return 0;
}

static void kthreadwork_exit(void)
{
        module_cleanup( );
	printk(KERN_INFO "Exiting kthreadwork module\n");
}

module_init(kthreadwork_init);
module_exit(kthreadwork_exit);
