/*  Snes Controller Driver
 *  A snes controller driver which uses the gpios of the raspberry pi
 *  to poll the information which will be transfered to the input subsystem
 *  event handlers
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/input.h>	// get into the input subsystem
#include <linux/input-polldev.h>	// controller data has to be polled
#include <linux/delay.h>	// sleep function


MODULE_AUTHOR("Harald Heckmann");
MODULE_DESCRIPTION("SNES Controller driver using RPis GPIOs.");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");

// mapping of buttons numbers to buttons
static const int button_mapping[12] = {BTN_X, BTN_A, BTN_SELECT,\
BTN_START, ABS_HAT0Y, ABS_HAT0Y, ABS_HAT0X, ABS_HAT0X,\
BTN_Y, BTN_B, BTN_TL, BTN_TR};

// struct needed for input.h (lib that actually includes our controller into the
// input subsystem)
 struct input_dev *snes_dev;
// struct needed for input-polldev.h (our device is old and needs to be polled)
//static struct input_polled_dev *snes_polled_dev;
int avrman_sensor = 0;

static void fill_input_dev(void) {
	snes_dev->name = "gsensor";
	// define EV_KEY type events
	//set_bit(EV_KEY, snes_dev->evbit);
	//set_bit(EV_ABS, snes_dev->evbit);
	//set_bit(BTN_B, snes_dev->keybit);
	//set_bit(BTN_Y, snes_dev->keybit);
	//set_bit(BTN_SELECT, snes_dev->keybit);
	//set_bit(BTN_START, snes_dev->keybit);
	//set_bit(BTN_X, snes_dev->keybit);
	//set_bit(BTN_A, snes_dev->keybit);
	//set_bit(BTN_TL, snes_dev->keybit);
	//set_bit(BTN_TR, snes_dev->keybit);
	
	set_bit(ABS_X, snes_dev->absbit);
	set_bit(ABS_Y, snes_dev->absbit);
	set_bit(ABS_Z, snes_dev->absbit);


	input_set_abs_params( snes_dev, ABS_X, -32768, 32767, 0, 0);
	input_set_abs_params(snes_dev, ABS_Y, -32768, 32767, 0, 0);
	input_set_abs_params(snes_dev, ABS_Z, -32768, 32767, 0, 0);
		
	// I could not find the product id
	snes_dev->id.bustype = BUS_HOST;
	snes_dev->id.vendor = 0x12E1;
	snes_dev->id.product = 0x0001;
	snes_dev->id.version = 0x001;
}

static int __init init_snes(void) {
	int error;

	printk(KERN_INFO "[SNES] initialising\n");
	printk(KERN_DEBUG "[SNES] Initialising GPIO-Pins...\n");


	printk(KERN_DEBUG "[SNES] Allocating structure input_dev...\n");
	snes_dev = input_allocate_device();
	if (!snes_dev) {
		printk(KERN_ERR "snes.c: Not enough memory\n");
		return -ENOMEM;
	}

	printk(KERN_DEBUG "[SNES] Filling structure input_dev...\n");
	fill_input_dev();

	printk(KERN_DEBUG "[SNES] Allocating structure snes_polled_dev...\n");

	printk(KERN_DEBUG "[SNES] Registering polled device...\n");
	
	error = input_register_device(snes_dev);
	if (error) {
		printk(KERN_ERR "snes.c: Failed to register polled device\n");
		return error;
	}
	
	printk(KERN_INFO "[SNES] initialised\n");
	
	avrman_sensor = 1;
	
	return 0;
}


static void __exit uninit_snes(void) {
	printk(KERN_INFO "[SNES] uninitialising\n");

	input_free_device(snes_dev);
	printk(KERN_INFO "[SNES] Good Bye Kernel :'(\n");
}


late_initcall(init_snes);
module_exit(uninit_snes);
