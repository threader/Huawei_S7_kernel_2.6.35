#if defined(CONFIG_MOUSE_OFN_HID) || defined(CONFIG_MOUSE_OFN_HID_MODULE)
#include <mach/ofn_hid.h> 
#endif

#if defined(CONFIG_SENSORS_AKM8973) || defined(CONFIG_SENSORS_AKM8973_MODULE)
#include <mach/akm8972_board.h>
#endif

#if HUAWEI_HWID_L2(S7, S7201)
#if defined(CONFIG_BATTERY_BQ275X0) ||defined(CONFIG_BATTERY_BQ275X0_MODULE)
#include <linux/i2c/bq275x0_battery.h>
#endif
#endif

#include "mach/msm_gsensor.h"
#include "mach/qsd_bcm4325.h"
#include "mach/msm_touch.h"

#if defined(CONFIG_HUAWEI_INCIDENT_LED)
#include "msm_incident_led.h"
#endif

#if defined(CONFIG_INPUT_PEDESTAL) || defined(CONFIG_INPUT_PEDESTAL_MODULE)
#include <mach/pedestal.h>
#endif

#if defined(CONFIG_DOCK_DET)
#include <linux/input/dock_det.h>
#endif

#if defined(CONFIG_CYPRESS120) || defined(CONFIG_CYPRESS120_MODULE)
#include <mach/cypress120_ts_dev.h>
#endif

#if HUAWEI_HWID_L2(S7, S7201)
#if defined(CONFIG_TOUCHSCREEN_T1320) || defined(CONFIG_TOUCHSCREEN_T1320_MODULE)
#include <linux/t1320.h>
#endif
#endif

#if defined(CONFIG_TOUCHSCREEN_MXT224) || defined(CONFIG_TOUCHSCREEN_MXT224_MODULE)
#include <linux/mxt224.h>
#endif
/*#if 1
#define DBG(fmt, args...) printk(KERN_INFO "[%s,%d] "fmt"\n", __FUNCTION__, __LINE__, ##args)
#else
#define DBG(fmt, args...) do {} while (0)
#endif*/

#define I(fmt, args...) printk(KERN_INFO "[%s,%d] "fmt"\n", __FUNCTION__, __LINE__, ##args)

#if HUAWEI_HWID_L2(S7, S7201)
#define GPIO_BS300_WAKEUP	103
#endif

#if HUAWEI_HWID(S70)
#if HWVERID_HIGHER(S70, B)
#define GPIO_BCM4325_WL_WAKE_HOST	38
#ifdef CONFIG_HUAWEI_CDMA_EVDO
#define GPIO_BCM4325_WL_RST_N		81
#else
#define GPIO_BCM4325_WL_RST_N		57
#endif
#elif HWVERID_HIGHER(S70, A)
#define GPIO_BCM4325_WL_WAKE_HOST	38
#define GPIO_BCM4325_WL_RST_N		17
#else
#define GPIO_BCM4325_WL_WAKE_HOST	34
#define GPIO_BCM4325_WL_RST_N		32
#endif

#if HWVERID_HIGHER(S70, T1)
#define GPIO_BCM4325_REG_ON			30
#define GPIO_BCM4325_HOST_WAKE_WL	109
#define GPIO_BCM4325_HOST_WAKE_BT	42
#define GPIO_BCM4325_BT_WAKE_HOST	21
#define GPIO_BCM4325_BT_RST_N		27
#define GPIO_GPS_LNA_EN				35
#define GPIO_GSENSOR_INT1			22
#define TOUCHPAD_SUSPEND 			34
#define TOUCHPAD_IRQ 				38
#endif

#if defined(CONFIG_MOUSE_OFN_HID) || defined(CONFIG_MOUSE_OFN_HID_MODULE)
#if HWVERID_HIGHER(S70, B)
#ifdef CONFIG_HUAWEI_CDMA_EVDO
#define GPIO_OPTNAV_IRQ             77
#else
#define GPIO_OPTNAV_IRQ             58
#endif
#define GPIO_OPTNAV_RESET           104
#define GPIO_OPTNAV_SHUTDOWN        105
#define GPIO_OPTNAV_DOME1           34
#else
#define GPIO_OPTNAV_IRQ             18
#define GPIO_OPTNAV_RESET           19
#define GPIO_OPTNAV_SHUTDOWN        20
#define GPIO_OPTNAV_DOME1           34
#endif
#endif

// add gpion define for compass
#if defined(CONFIG_SENSORS_AKM8973) || defined(CONFIG_SENSORS_AKM8973_MODULE)
#if HWVERID_HIGHER(S70, B)
#define GPIO_COMPASS_IRQ             20
#define GPIO_COMPASS_RESET           18
#else
#error -- No compass driver
#endif
#endif

#if defined(CONFIG_DOCK_DET) || defined(CONFIG_DOCK_DET_MODULE)
#define GPIO_DOCK_DET	(106)
#endif

#if defined(CONFIG_CYPRESS120) || defined(CONFIG_CYPRESS120_MODULE)
#define GPIO_CTP_INT   			(28)
#define GPIO_CTP_POWER   		(101)
#define GPIO_CTP_RESET       	(158)
#define CTP_I2C_ADDR   			(0x26)
#endif

#if defined(CONFIG_GPIO_I2C_ADPT)
#define GPIO_I2C_SCL		98
#define GPIO_I2C_SDA		100
#endif
#endif
