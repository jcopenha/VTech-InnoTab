/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2010 by Generalplus Inc.                         *
 *                                                                        *
 *  This software is copyrighted by and is the property of Generalplus    *
 *  Inc. All rights are reserved by Generalplus Inc.                      *
 *  This software may only be used in accordance with the                 *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Generalplus Technology Co., Ltd.                   *
 *                                                                        *
 *  Generalplus Inc. reserves the right to modify this software           *
 *  without notice.                                                       *
 *                                                                        *
 *  Generalplus Inc.                                                      *
 *  3F, No.8, Dusing Rd., Hsinchu Science Park,                           *
 *  Hsinchu City 30078, Taiwan, R.O.C.                                    *
 *                                                                        *
 **************************************************************************/
/**
 * @file    gp_i2c_bus.c
 * @brief   Implement of i2c bus driver.
 * @author  junp.zhang
 * @since   2010/10/12
 * @date    2010/10/12
 */
#include <mach/kernel.h>
#include <mach/module.h>
#include <mach/cdev.h>
#include <mach/diag.h>
#include <mach/gp_i2c_bus.h>
#include <mach/hal/hal_i2c_bus.h>
#include <mach/hal/hal_clock.h>

//Added by Kevin
#include <mach/hal/hal_gpio.h>
#include <mach/gp_gpio.h>
#include <linux/delay.h> 	/* udelay/mdelay */

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define I2C_BUS_WRITE 0
#define I2C_BUS_READ  1

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
 typedef struct i2c_bus_info_s {
	struct miscdevice dev;          /*!< @brief i2c bus device */
	struct semaphore sem;           /*!< @brief mutex semaphore for i2c bus ops */
	unsigned int open_count;        /*!< @brief i2c bus device open count */
} i2c_bus_info_t;

typedef struct i2c_bus_handle_s{
	int slaveAddr;				/*!< @brief slave device address*/
	int clkRate;				/*!< @brief i2c bus clock rate */
}i2c_bus_handle_t;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static i2c_bus_info_t *i2c_bus = NULL;

/**
 * @brief   I2c clock enable/disable
 * @param   enable [in] 0:disable, 1:enable
 * @return  None
 * @see
 */
static void i2c_clock_enable(int enable)
{
	gpHalScuClkEnable(SCU_C_PERI_I2C, SCU_C, enable);
}

/**
 * @brief   I2C bus request function.
 * @param   slave_addr[in]: slave device ID
 * @param   clk_rate[in]: i2c bus clock rate(KHZ)
 * @return  i2c bus handle/ERROR_ID
 * @see
 */
int gp_i2c_bus_request(int slave_addr, int clk_rate)
{
	int ret = 0;
	i2c_bus_handle_t *hd = NULL;
	
	hd = (i2c_bus_handle_t *)kmalloc(sizeof(i2c_bus_handle_t), GFP_KERNEL);
	if (NULL == hd) {
		ret = -ENOMEM;
		goto __err_kmalloc;
	}
	
	memset(hd, 0, sizeof(i2c_bus_handle_t));
	
	hd->slaveAddr = slave_addr;
	hd->clkRate = clk_rate;
	
	if( i2c_bus->open_count == 0) {
	i2c_clock_enable(1);
	}
	i2c_bus->open_count++;
	
	/*init i2c bus register*/
	gpHalI2cBusInit();
	
	ret = (int)hd;
	return ret;
	
__err_kmalloc :
	return ret;
}
EXPORT_SYMBOL(gp_i2c_bus_request);

/**
 * @brief   I2C bus release function.
 * @param   handle[in]: i2c bus handle
 * @return  SP_OK(0)/SP_FAIL(1)
 * @see
 */
int gp_i2c_bus_release(int handle)
{
	int ret = SP_FAIL;

 	i2c_bus_handle_t *hd = (i2c_bus_handle_t *)handle;
 	if (NULL != hd) {
     kfree(hd);
     hd = NULL;
     ret = SP_OK;
 	}
 	
	i2c_bus->open_count--;
	if( i2c_bus->open_count == 0 ){
	i2c_clock_enable(0);
	}

 	return ret;
}
EXPORT_SYMBOL(gp_i2c_bus_release);

/**
 * @brief   I2C bus data transfer function
 * @param   handle[in]: i2c bus handle
 * @param   data[in]: data buffer
 * @param   len[in]: data length
 * @param   cmd[in]: I2C_BUS_WRITE/I2C_BUS_READ
 * @return  data length/ERROR_ID
 */
static int i2c_bus_xfer(int handle, unsigned char* data, unsigned int len, int cmd)
{
	int ret = -ENXIO;
	int i = 0;
	i2c_bus_handle_t *hd = NULL;
	
	hd = (i2c_bus_handle_t *)handle;
	
	if (hd->clkRate <= 0) {
		DIAG_ERROR("ERROR: i2c clock rate must to be more than zero\n");
		return -EINVAL;
	}
	
//	if (down_interruptible(&i2c_bus->sem) != 0) {
//		return -ERESTARTSYS;
//	}
	if (down_trylock(&i2c_bus->sem) != 0) {//Modified by Kevin
		return 0;
	}
	
	if (cmd == I2C_BUS_WRITE) {
		ret = gpHalI2cBusStartTran(hd->slaveAddr, hd->clkRate, I2C_BUS_WRITE, 0);
		if (ret == SP_FAIL) {
			DIAG_ERROR("ERROR: write slave device address fail\n");
			goto out;
		}
	
	for (i = 0; i < len; i++) {
		ret = gpHalI2cBusMidTran(&data[i], I2C_BUS_WRITE, 0);
		if (ret == SP_FAIL) {
			DIAG_ERROR("ERROR: write data fail\n");
			goto out;	
		}
	}
	
		gpHalI2cBusStopTran(I2C_BUS_WRITE);	
	
	} else {
		ret = gpHalI2cBusStartTran(hd->slaveAddr, hd->clkRate, I2C_BUS_READ, 1);
		if (ret == SP_FAIL) {
			DIAG_ERROR("ERROR: write slave device address fail\n");
			goto out;
		}
	
	  for (i = 0; i < len; i++) {
		if(i == (len - 1)) {
		  ret = gpHalI2cBusMidTran(&data[i], I2C_BUS_READ, 0);
			} else {
		  ret = gpHalI2cBusMidTran(&data[i], I2C_BUS_READ, 1);
			}
			
			if (ret == SP_FAIL) {
					DIAG_ERROR("ERROR: read data fail\n");
					goto out;
			}
		}

		gpHalI2cBusStopTran(I2C_BUS_READ);
	}
	
	ret = i;
	up(&i2c_bus->sem);

	return ret;
	
out:
	up(&i2c_bus->sem);
	ret = -ENXIO;
	return ret;
}	

/**
 * @brief   I2C bus write function
 * @param   handle[in]: i2c bus handle
 * @param   data[in]: data to write
 * @param   len[in]: data length
 * @return  data length/ERROR_ID
 */
int gp_i2c_bus_write(int handle, unsigned char* data, unsigned int len)
{
	return	i2c_bus_xfer(handle, data, len, I2C_BUS_WRITE);
}
EXPORT_SYMBOL(gp_i2c_bus_write);

/**
 * @brief   I2C bus read function
 * @param   handle[in]: i2c bus handle
 * @param   data[out]: data buffer
 * @param   len[in]: data length
 * @return  data length/ERROR_ID
 */
int gp_i2c_bus_read(int handle, unsigned char* data, unsigned int len)
{
	return	i2c_bus_xfer(handle, data, len, I2C_BUS_READ);
}
EXPORT_SYMBOL(gp_i2c_bus_read);

/**
 * @brief   I2C bus device open function
 */
static int i2c_bus_open(struct inode *inode, struct file *flip)
{
	int ret = 0;
	
	if (down_interruptible(&i2c_bus->sem) != 0) {
		return -ERESTARTSYS;
	}

	ret = gp_i2c_bus_request(-1, -1);
	if (IS_ERR_VALUE(ret)) {
		DIAG_ERROR("ERROR: request i2c bus fail\n");
		up(&i2c_bus->sem);
		return ret;	
	}
	flip->private_data = (i2c_bus_handle_t *)ret;

	up(&i2c_bus->sem);
	
	return 0;
}

/**
 * @brief   I2C bus device ioctl function
 */
static int i2c_bus_ioctl(struct inode *inode, struct file *flip, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	i2c_bus_handle_t *hd = NULL;
	
	hd = (i2c_bus_handle_t *)flip->private_data;
	
	switch(cmd){
	case I2C_BUS_ATTR_SET:{
		i2c_bus_handle_t handle;
		memset(&handle, 0, sizeof(handle));
			
		if (copy_from_user(&handle, (void __user*)arg, sizeof(handle))) {
			return -EFAULT;
		}
		hd->slaveAddr = handle.slaveAddr;
		hd->clkRate = handle.clkRate;
		}
			break;
	default:
		ret = -ENOTTY;
		break;
	}
	
	return ret;
}

/**
 * @brief   I2C bus device write function
 */
static ssize_t i2c_bus_write(struct file *flip, const char __user *buf, size_t count, loff_t *oppos)
{
	int ret = 0;
	unsigned char *sendBuf = NULL;
	
	sendBuf = (unsigned char *)kmalloc(count, GFP_KERNEL);
	if (!sendBuf) {
		ret = -ENOMEM;
		goto __err_kmalloc;
	}
	
	if (copy_from_user(sendBuf, buf, count)) {
		ret = -EFAULT;
		goto __err_copy;
	}

	ret = gp_i2c_bus_write((int)flip->private_data, sendBuf, count);
	if (ret < 0) {
		DIAG_ERROR("ERROR: i2c bus write fail\n");
		//return -ENXIO; /*MUST free sendBuf*/
		ret = -ENXIO;
	}

__err_copy:
	kfree(sendBuf);
	sendBuf = NULL;
__err_kmalloc:
	return ret;
}

/**
 * @brief   I2C bus device read function
 */
static ssize_t i2c_bus_read(struct file *flip, char __user *buf, size_t count, loff_t *oppos)
{
	int ret = 0;
	unsigned char *recvBuf = NULL;
			
	recvBuf = (unsigned char *)kmalloc(count, GFP_KERNEL);
	if (!(recvBuf)) {
		ret = -ENOMEM;
		goto __err_kmalloc;
	}

	ret = gp_i2c_bus_read((int)flip->private_data, recvBuf, count);
	if (ret <= 0) {
		DIAG_ERROR("ERROR: i2c read fail\n");
		/*return -ENXIO;*/
		ret = -ENXIO;
		goto __err_copy;
	}
	
	if (copy_to_user(buf, recvBuf, ret)) {
		ret = -EFAULT;
		/*goto __err_copy;*/
	}

__err_copy:
	kfree(recvBuf);
	recvBuf = NULL;
__err_kmalloc:
	return ret;
}

/**
 * @brief   I2C bus device release function
 */
static int i2c_bus_release(struct inode *inode, struct file *flip)
{
	int ret = -ENXIO;
	
	ret = gp_i2c_bus_release((int)flip->private_data);
	if (ret == SP_FAIL) {
		DIAG_ERROR("ERROR: gp i2c release\n");
		return ret;
	}
	
	if (down_interruptible(&i2c_bus->sem) != 0) {
		return -ERESTARTSYS;
	}
	
	if (0 == i2c_bus->open_count) {
		ret = SP_OK;
	}

	up(&i2c_bus->sem);
	return ret;
}

static const struct file_operations i2c_bus_fops = {
	.owner 		= THIS_MODULE,
	.open 		= i2c_bus_open,
	.release 	= i2c_bus_release,
	.ioctl 		= i2c_bus_ioctl,
	.read 		= i2c_bus_read,
	.write 		= i2c_bus_write,
};

/**
 * @brief   i2c device release
 */
static void gp_i2c_device_release(struct device *dev)
{
	DIAG_INFO("remove i2c device ok\n");
}

static struct platform_device gp_i2c_device = {
	.name	= "gp-i2c",
	.id	= 0,
	.dev	= {
		.release = gp_i2c_device_release,
	}
};

//Added by Kevin, to release previous BUSY state
static int i2c_force_release(void)
{
	unsigned int pinindex_scl, pinindex_sda, count, value;
	int ret = 0;

//Set to GPIO function
	pinindex_sda = MK_GPIO_INDEX(1, 3, 13, 16);
	pinindex_scl = MK_GPIO_INDEX(1, 3, 13, 17);
	//GPB16, GPIO, input, pull-high
	gpHalGpioSetPadGrp(pinindex_sda);
	gpHalGpioSetFunction(pinindex_sda, GPIO_FUNC_GPIO);
	gpHalGpioSetDirection(pinindex_sda,GPIO_DIR_INPUT);
	gpHalGpioSetPullFunction(pinindex_sda, GPIO_PULL_HIGH);
	//GPB17, GPIO, output, high
	gpHalGpioSetPadGrp(pinindex_scl);
	gpHalGpioSetFunction(pinindex_scl, GPIO_FUNC_GPIO);
	gpHalGpioSetDirection(pinindex_scl,GPIO_DIR_OUTPUT);
	gpHalGpioSetDrivingCurrent(pinindex_scl,1);
	gpHalGpioSetValue(pinindex_scl, 1);

	value = count = 0;
	gpHalGpioGetValue(pinindex_sda, &value);
	if (value == 0) {
		printk("######## I2C bus BUSY detected !!!!!!!!! Try to release it...\n");
	}
	while ((value == 0) && (count++ < 16)) {
		//generate a pulse at SCL pin to let slave device release SDA pin
		gpHalGpioSetValue(pinindex_scl, 0);
		udelay(50);
		gpHalGpioSetValue(pinindex_scl, 1);
		udelay(50);
		gpHalGpioGetValue(pinindex_sda, &value);	
	}

	if (value == 0) {
		printk("######## Release SDA pin fail !!! try_count = %d\n", count);
		ret = -EFAULT;
	} else if (count > 0) {
		//generate stop condition
		gpHalGpioSetDirection(pinindex_sda, GPIO_DIR_OUTPUT);
		gpHalGpioSetValue(pinindex_sda, 0);
		udelay(50);
		gpHalGpioSetValue(pinindex_sda, 1);
		udelay(50);
		
		printk("######## Release SDA pin successfully!! try_count = %d\n", count);
		ret = 0;
	}

//Restore to I2C function
	pinindex_sda = MK_GPIO_INDEX(1, 0, 13, 16);
	pinindex_scl = MK_GPIO_INDEX(1, 0, 13, 17);
	//GPB16, I2C-SDA, input, pull-high
	gpHalGpioSetPadGrp(pinindex_sda);
	gpHalGpioSetFunction(pinindex_sda, GPIO_FUNC_ORG);
	gpHalGpioSetDirection(pinindex_sda,GPIO_DIR_INPUT);
	gpHalGpioSetPullFunction(pinindex_sda, GPIO_PULL_HIGH);
	//GPB17, I2C-SCL, input, high
	gpHalGpioSetPadGrp(pinindex_scl);
	gpHalGpioSetFunction(pinindex_scl, GPIO_FUNC_ORG);
	gpHalGpioSetDirection(pinindex_scl,GPIO_DIR_INPUT);
	gpHalGpioSetPullFunction(pinindex_scl, GPIO_PULL_HIGH);
	
	return ret;
}

static int gp_i2c_probe(struct platform_device *pdev){
//	return 0;
	return i2c_force_release(); //Modified by Kevin
}

static int gp_i2c_remove(struct platform_device *pdev){
	return 0;
}



#ifdef CONFIG_PM
static int gp_i2c_suspend(struct platform_device *pdev, pm_message_t state){
	if (i2c_bus->open_count > 0) {
	i2c_clock_enable(0);
	}
	return 0;
}

static int gp_i2c_resume(struct platform_device *pdev){
	i2c_force_release();//Added by Kevin
	if( i2c_bus->open_count > 0) {
	i2c_clock_enable(1);
	/*init i2c bus register*/
	gpHalI2cBusInit();
	}
	return 0;
}
#else
#define gp_i2c_suspend NULL
#define gp_i2c_resume NULL
#endif

/**
 * @brief   wdt driver define
 */
static struct platform_driver gp_i2c_driver = {
	.probe  = gp_i2c_probe,
	.remove = gp_i2c_remove,
	.suspend = gp_i2c_suspend,
	.resume = gp_i2c_resume,
	.driver	= {
		.owner	= THIS_MODULE,
		.name	= "gp-i2c"
	}
};

/**
 * @brief   I2C bus driver init function
 */
static int  __init i2c_bus_init(void)
{
	int ret = 0;

	i2c_bus = (i2c_bus_info_t *)kmalloc(sizeof(i2c_bus_info_t),  GFP_KERNEL);
	if (NULL == i2c_bus) {
		ret = -ENOMEM;
		DIAG_ERROR("ERROR: kmalloc fail\n");
		goto __err_kmalloc;
	}
	memset(i2c_bus, 0, sizeof(i2c_bus_info_t));
	
	/* initialize */
	i2c_bus->open_count = 0;
	init_MUTEX(&i2c_bus->sem);

	i2c_bus->dev.name  = "i2c";
	i2c_bus->dev.minor = MISC_DYNAMIC_MINOR;
	i2c_bus->dev.fops  = &i2c_bus_fops;

	/* register device */
	ret = misc_register(&i2c_bus->dev);
	if (ret != 0) {
		DIAG_ERROR("ERROR: device register fail\n");
		goto __err_device_register;
	}

	platform_device_register(&gp_i2c_device);
	return platform_driver_register(&gp_i2c_driver);
	
/* error rollback */
__err_device_register:
	misc_deregister(&i2c_bus->dev);
	kfree(i2c_bus);
	i2c_bus = NULL;	
__err_kmalloc:
	return ret;
}

/**
 * @brief   I2C bus driver init function
 */
static void __exit i2c_bus_exit(void)
{
	misc_deregister(&i2c_bus->dev);
	i2c_clock_enable(0);
	kfree(i2c_bus);
	i2c_bus = NULL;

	platform_device_unregister(&gp_i2c_device);
	platform_driver_unregister(&gp_i2c_driver);
}

module_init(i2c_bus_init);
module_exit(i2c_bus_exit);

/**************************************************************************
 *                  M O D U L E    D E C L A R A T I O N                  *
 **************************************************************************/

MODULE_AUTHOR("Generalplus");
MODULE_DESCRIPTION("Generalplus I2c Bus Driver");
MODULE_LICENSE_GP;



