#include <linux/init.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
 

static ssize_t device_file_read(
                        struct file *file_ptr
                       , char __user *user_buffer
                       , size_t count
                       , loff_t *position);
static int mydev_open ( struct inode *inode , struct file *filep );
static int mydev_release ( struct inode *inode , struct file *filep );
static ssize_t mydev_write (
                        struct file *file_ptr
                       , const char __user *user_buffer
                       , size_t count
                       , loff_t *position);
 
static int device_file_major_number = 0;
static const char device_name[] = "Simple-driver";

struct file_operations simple_driver_fops = {
    owner: THIS_MODULE,
    open: mydev_open,
    release: mydev_release,
    read: device_file_read,
    write: mydev_write,
};

int register_device(void)
{
        int result = 0;
        printk( KERN_NOTICE "Simple-driver: register_device() is called.\n" );
        result = register_chrdev( 0, device_name, &simple_driver_fops );
        if( result < 0 )
        {
            printk( KERN_WARNING "Simple-driver:  can\'t register character device with errorcode = %i \n", result );
            return result;
        }
        device_file_major_number = result;
        printk( KERN_NOTICE "Simple-driver: registered character device with major number = %i and minor numbers 0...255\n"
             , device_file_major_number );
        return 0;
}

void unregister_device(void)
{
    printk( KERN_INFO "Simple-driver: unregister_device() is called\n" );
    if(device_file_major_number != 0)
    {
        unregister_chrdev(device_file_major_number, device_name);
    }
}
 
static int my_init(void)
{
	printk(KERN_NOTICE "registering device\n");
	register_device();
                       return  0;
}
 
static void my_exit(void)
{
	printk(KERN_NOTICE "unregistering device\n");
	unregister_device();
                       return;
}
 
module_init(my_init);
module_exit(my_exit);


static const char    g_s_Hello_World_string[] = "Hello world from kernel mode!\n\0";
static const ssize_t g_s_Hello_World_size = sizeof(g_s_Hello_World_string);
static ssize_t device_file_read(
                        struct file *file_ptr
                       , char __user *user_buffer
                       , size_t count
                       , loff_t *position)
{
    printk( KERN_NOTICE "Simple-driver: Device file is read at offset = %i, read bytes count = %u \n"
                , (int)*position
                , (unsigned int)count );
    /* If position is behind the end of a file we have nothing to read */
    if( *position >= g_s_Hello_World_size )
        return 0;
    /* If a user tries to read more than we have, read only as many bytes as we have */
     
    if( *position + count > g_s_Hello_World_size )
        count = g_s_Hello_World_size - *position;
    if( copy_to_user(user_buffer, g_s_Hello_World_string + *position, count) != 0 )
        return -EFAULT;    
    /* Move reading position */
    *position += count;
    return count;
}

static int mydev_open ( struct inode *inode , struct file *filep )
{
printk( KERN_NOTICE "Simple-driver: Device opened \n");
return 0;
}

static int mydev_release ( struct inode *inode , struct file *filep )
{
printk( KERN_NOTICE "Simple-driver: Device closed \n ");
return 0;
}

static ssize_t mydev_write (
                        struct file *file_ptr
                       , const char __user *user_buffer
                       , size_t count
                       , loff_t *position)
{
printk( KERN_NOTICE "Simple-driver: Writing to the device ...\n ");
return 1;
}




MODULE_LICENSE (" OPEN ");
MODULE_AUTHOR (" Vishnu Kumar ");
MODULE_DESCRIPTION (" Test Driver Module ");
