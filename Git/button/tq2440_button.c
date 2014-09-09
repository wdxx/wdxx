
/***************************************************************************************
****************************************************************************************
* FILE		: tq2440_button.c
* Description	: 
*			  
* Copyright (c) 2014 by XXX. All Rights Reserved.
* 
* History:
* Version		Name       		Date			Description
   0.1		XXX	2014/09/09	Initial Version
   
****************************************************************************************
****************************************************************************************/
#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>

#include <asm/irq.h>

static struct input_dev *button_dev;

static irqreturn_t button_interrupt(int irq, void *dummy)
{
	input_report_key(button_dev, BTN_BACK, inb(BUTTON_PORT) & 1);
	input_sync(button_dev);
	return IRQ_HANDLED;
}


static int button_init(void)
{
    int error;

    error = request_irq(IRQ_EINT1, button_interrupt, 0, "tq_button1",NULL);
    if (error != 0)
    {
        printk(KERN_ERR "button.c: Can't allocate irq %d\n", IRQ_EINT1);
        return -EBUSY;
    }

    button_dev = input_allocate_device();
    if (button_dev == NULL)
    {
        printk(KERN_ERR "button.c: Not enough memory\n");
        error = -ENOMEM;
        goto err_free_irq;
    }

    //button_dev->evbit[0] = BIT_MASK(EV_KEY);
    //button_dev->keybit[BIT_WORD(BTN_BACK)] = BIT_MASK(BTN_BACK);
    set_bit(EV_KEY, button_dev->evbit);
    set_bit(BTN_BACK, button_dev->keybit);

    error = input_register_device(button_dev);

    if (error) 
    {
        printk(KERN_ERR "button.c: Failed to register device\n");
        goto err_free_dev;
    }
    
    return 0;

err_free_dev:
    input_free_device(button_dev);

err_free_irq:
    free_irq(IRQ_EINT1, button_interrupt);


return error;    
}


static void button_exit(void)
{
    input_unregister_device(button_dev);
    free_irq(IRQ_EINT1, button_interrupt);
}

module_init(button_init);
module_exit(button_exit);

