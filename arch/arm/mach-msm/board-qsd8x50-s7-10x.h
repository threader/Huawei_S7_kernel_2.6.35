// BCM
#define MODE_BCM4325_REG_ON_LEVEL		0
#define MODE_BCM4325_WL_RST_N_LEVEL		1
#define MODE_BCM4325_HOST_WAKE_WL		2

static unsigned int bcm4325_level_status = 0;
static int bcm4325_init_flag = 0;
static struct semaphore bcm4325_pmsem;

static uint32_t msm_pm_setup_status(struct device *dv, unsigned int vdd)
{
	struct platform_device *pdev;

	pdev = container_of(dv, struct platform_device, dev);
    down(&sdcc_sts_sem);
	msm_sdcc_setup_gpio(pdev->id, !!vdd);
    mmc_delay(10);

    /* 
     * !!! we should always keep SD Card(VREG_P6) on because of bug of PMIC7540 
     *     and avoiding SD Card re-detect by vold result in causing remounting, 
     *     sounding and screen on
     */
#ifdef CONFIG_MMC_MSM_NOT_REMOVE_CARD_WHEN_SUSPEND
	vreg_mmc_control(pdev->id, 1);
#else
	vreg_mmc_control(pdev->id, !!vdd);
#endif

    up(&sdcc_sts_sem);

	return 0;
}

#if defined(CONFIG_EMBEDDED_SDIO_DATA)
static struct sdio_embedded_func wlan_func = {
	.f_class		= SDIO_CLASS_WLAN,
	.f_maxblksize	= 64,
};
#endif

static void (*wlan_status_cb)(int card_present, void *dev_id);
static void *wlan_status_cb_devid;
static int qsd8x50_wlan_cd = 0;

static int qsd8x50_register_status_notify(void (*callback)(int card_present,
							  							   void *dev_id),
					 					  void *dev_id)
{
	if (wlan_status_cb)
		return -EAGAIN;
	wlan_status_cb = callback;
	wlan_status_cb_devid = dev_id;
	return 0;
}

static unsigned int qsd8x50_wlan_status(struct device *dev)
{
	return qsd8x50_wlan_cd;
}

int qsd8x50_wlan_set_carddetect(int val)
{
	qsd8x50_wlan_cd = val;
	if (wlan_status_cb)
		wlan_status_cb(val, wlan_status_cb_devid);
	else
		I("Nobody to notify");
	return 0;
}
EXPORT_SYMBOL(qsd8x50_wlan_set_carddetect);

#if 0
int qsd8x50_wlan_get_carddetect(void)
{
	return qsd8x50_wlan_status(NULL);
}
EXPORT_SYMBOL(qsd8x50_wlan_get_carddetect);
#endif

static int bcm4325_pm_init(void)
{
	int rc;

	if ((rc = gpio_request(GPIO_BCM4325_HOST_WAKE_WL, "bcm4325_wl_waked"))) {
	    pr_err("gpio_request failed on pin %d\n", GPIO_BCM4325_HOST_WAKE_WL);
	    goto err_bcm4325_pm_init2;
	}
    
	if ((rc = gpio_request(GPIO_BCM4325_WL_WAKE_HOST, "bcm4325_wl_wake_host"))) {
	    pr_err("gpio_request failed on pin %d\n", GPIO_BCM4325_WL_WAKE_HOST);
	    goto err_bcm4325_pm_init3;
	}
    
	if ((rc = gpio_request(GPIO_BCM4325_WL_RST_N, "bcm4325_wl_rstn"))) {
	    pr_err("gpio_request failed on pin %d\n", GPIO_BCM4325_WL_RST_N);
	    goto err_bcm4325_pm_init4;
	}

    init_MUTEX(&bcm4325_pmsem);
    if (down_interruptible(&bcm4325_pmsem))
		return -1;
    bcm4325_init_flag = 1;
    bcm4325_level_status = 0;
    up(&bcm4325_pmsem);

	return 0;

	gpio_free(GPIO_BCM4325_WL_RST_N);    
err_bcm4325_pm_init4:
	gpio_free(GPIO_BCM4325_WL_WAKE_HOST);
err_bcm4325_pm_init3:
	gpio_free(GPIO_BCM4325_HOST_WAKE_WL);
err_bcm4325_pm_init2:
    bcm4325_init_flag = 0;
    return -1;
}

/* power control of wifi, on_level = 1: power on, 2: reset on, 3: wake on */
void bcm_wlan_power_on(int on_level)
{
	int rc;

	unsigned mpp_wl_reg_on;
#if HWVERID_HIGHER(S70, A)
    mpp_wl_reg_on = 3 - 1;
#else
    mpp_wl_reg_on = 19 - 1;
#endif

	pr_info("[%s,%d] on_level = %d\n", __FUNCTION__, __LINE__, on_level);

	/* put HOST_WAKE_WL to HIGH */
	if (on_level == 1 || on_level == 3) {
		rc = gpio_tlmm_config(GPIO_CFG(GPIO_BCM4325_HOST_WAKE_WL, 0, GPIO_CFG_OUTPUT, 
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		if (rc) 
			printk(KERN_ERR "%s: config HOST_WAKE_WL failed: %d\n",__func__, rc);
		gpio_set_value(GPIO_BCM4325_HOST_WAKE_WL, 1);
		if (on_level == 3)
			return;
	}

	if (on_level == 1) {
		rc = mpp_config_digital_out(mpp_wl_reg_on,
				     	MPP_CFG(MPP_DLOGIC_LVL_MSMP,
				     	MPP_DLOGIC_OUT_CTRL_HIGH));
		if (rc) 
			printk(KERN_ERR "%s: config WL_REG_ON failed: %d\n",__func__, rc);

		mmc_delay(160);

		/* gpio 32 is for WL_WOW - WiFi reset */
		rc = gpio_tlmm_config(GPIO_CFG(GPIO_BCM4325_WL_RST_N, 0, GPIO_CFG_OUTPUT, 
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		if (rc) {
			printk(KERN_ERR "%s: config WL_WOW failed: %d\n",__func__, rc);
		}
		gpio_set_value(GPIO_BCM4325_WL_RST_N, 1);
		/* reset again */
/* reduce response time */
#if 0
		mmc_delay(160);
		gpio_set_value(GPIO_BCM4325_WL_RST_N, 0);
		mmc_delay(50);
		gpio_set_value(GPIO_BCM4325_WL_RST_N, 1);
#endif
	} else if (on_level == 2) {
		rc = gpio_tlmm_config(GPIO_CFG(GPIO_BCM4325_WL_RST_N, 0, GPIO_CFG_OUTPUT, 
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
		if (rc) 
			printk(KERN_ERR "%s: config WLAN_WOW failed: %d\n",__func__, rc);

		gpio_set_value(GPIO_BCM4325_WL_RST_N, 1);
	}

	mmc_delay(160);
}
EXPORT_SYMBOL(bcm_wlan_power_on);

/* power control of wifi, off_level = 1: power off, 2: reset off, 3: wake off */
void bcm_wlan_power_off(int off_level)
{
	int rc;

	unsigned mpp_wl_reg_on;

#if HWVERID_HIGHER(S70, A)
	mpp_wl_reg_on = 3 - 1;
#else
    mpp_wl_reg_on = 19 - 1;
#endif

	pr_info("[%s,%d] off_level = %d\n", __FUNCTION__, __LINE__, off_level);

	if (off_level == 1) {
		/*gpio 32 is  WLAN_WOW - WiFi Reset*/
		gpio_set_value(GPIO_BCM4325_WL_RST_N, 0);
		rc = gpio_tlmm_config(GPIO_CFG(GPIO_BCM4325_WL_RST_N, 0, GPIO_CFG_OUTPUT, 
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_DISABLE);
		if (rc)
			printk(KERN_ERR "%s: config WLAN_WOW failedi: %d\n", __func__, rc);

		rc = mpp_config_digital_out(mpp_wl_reg_on,
				     	MPP_CFG(MPP_DLOGIC_LVL_MSMP,
				     	MPP_DLOGIC_OUT_CTRL_LOW));
		if (rc)
     		printk(KERN_ERR "%s: config WL_REG_ON failed: %d\n",__func__, rc);
	} else if (off_level == 2) {
		gpio_set_value(GPIO_BCM4325_WL_RST_N, 0);
		rc = gpio_tlmm_config(GPIO_CFG(GPIO_BCM4325_WL_RST_N, 0, GPIO_CFG_OUTPUT, 
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_DISABLE);
		if (rc)
			printk(KERN_ERR "%s: config WLAN_WOW failedi: %d\n", __func__, rc);
	}

	/* put HOST_WAKE_WL to LOW */
	if (off_level == 1 || off_level == 3) {
		gpio_set_value(GPIO_BCM4325_HOST_WAKE_WL, 0);
		rc = gpio_tlmm_config(GPIO_CFG(GPIO_BCM4325_HOST_WAKE_WL, 0, GPIO_CFG_OUTPUT, 
					GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_DISABLE);
		if (rc)
			printk(KERN_ERR "%s: config WLAN_WOW failedi: %d\n", __func__, rc);

		if (off_level == 3)
			return;
	}
}
EXPORT_SYMBOL(bcm_wlan_power_off);

// END BCM