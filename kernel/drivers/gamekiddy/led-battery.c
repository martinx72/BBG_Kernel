#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/module.h> 
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/power_supply.h>

void avrman_led_init( int capacity );
int bq2589x_get_charging_status( void );
 
extern struct gpio_desc *charger_led1;

int led_thread_function(void *data)
{

//	int ret;
	int invert=0;

	int capacity;
	int avrman_state = 0;
		
	union power_supply_propval ret = {0,};
	
	struct power_supply *batt_psy;
	
	batt_psy = power_supply_get_by_name("battery");
	
	if ( batt_psy ) {
		batt_psy->desc->get_property(batt_psy,POWER_SUPPLY_PROP_CAPACITY,&ret);
		capacity =  ret.intval;
	} else {
		capacity =  50;
	}
	
	avrman_led_init( capacity );

	while(1)
        {
		avrman_state = bq2589x_get_charging_status();
		
		batt_psy->desc->get_property(batt_psy,POWER_SUPPLY_PROP_CAPACITY,&ret);
		capacity =  ret.intval;

		if ( capacity > 5 )
		{
			gpiod_set_value( charger_led1 ,0);
		}
		else
		{

			if( avrman_state == 1 || avrman_state == 2 )
				invert = 0;
			else
				invert=~invert;
				
			gpiod_set_value( charger_led1 ,invert);	
		}
		
		msleep(1000);
        }
}


static int __init led_battery_init(void)
{

	struct task_struct *tsk;
	tsk = kthread_run(led_thread_function, NULL, "battery led thread");

	return 0;
}

static void __exit led_battery_exit(void)
{
}


module_init(led_battery_init);


module_exit(led_battery_exit);

MODULE_DESCRIPTION("");
MODULE_AUTHOR("");
MODULE_LICENSE("GPL");

