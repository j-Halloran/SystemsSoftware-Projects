/*
* Kernel module to generate random dice rolls
* Author Jake Halloran
* Last Edited: 4/10/16
*/

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/random.h>
#include <asm/uaccess.h>

//Helper function to get a single random byte
unsigned char get_random_byte(int max) {
        unsigned char c;
        get_random_bytes(&c, 1);
        return c%max;
}

/*
 * dice_read is the function called when a process calls read() on
 * /dev/dice. Writes a random byte from 0 to 5 on the return buffer
 */
static ssize_t dice_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos)
{
	char temp = (char) get_random_byte(5);
	char * dice_str = &temp;
	int len = 1; /* Don't include the null byte. */
	
	/*
	 * Besides copying the string to the user provided buffer,
	 * this function also checks that the user has permission to
	 * write to the buffer, that it is mapped, etc.
	 */
	if (copy_to_user(buf, dice_str, len))
		return -EINVAL;
	/*
	 * Tell the user how much data we wrote.
	 */
	*ppos = len;

	return len;
}




/*
 * The only file operation we care about is read.
 */

static const struct file_operations dice_fops = {
	.owner		= THIS_MODULE,
	.read		= dice_read,
};

static struct miscdevice dice_driver = {
	/*
	 * We don't care what minor number we end up with, so tell the
	 * kernel to just pick one.
	 */
	MISC_DYNAMIC_MINOR,
	/*
	 * Name ourselves /dev/dice.
	 */
	"dice",
	/*
	 * What functions to call when a program performs file
	 * operations on the device.
	 */
	&dice_fops
};

static int __init
dice_init(void)
{
	int ret;

	/*
	 * Create the "dice" device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/dice device using
	 * the default rules.
	 */
	ret = misc_register(&dice_driver);
	if (ret)
		printk(KERN_ERR
		       "Unable to register \"dice roll generator\" misc device\n");

	return ret;
}

module_init(dice_init);

static void __exit
dice_exit(void)
{
	misc_deregister(&dice_driver);
}

module_exit(dice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jake Halloran <jph74@pitt.edu>");
MODULE_DESCRIPTION("\"Random Dice Roll Module");
MODULE_VERSION("dev");
