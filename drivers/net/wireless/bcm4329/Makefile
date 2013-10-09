# bcm4329
DHDCFLAGS =  -Wall -Wstrict-prototypes -DLINUX -DBCMDRIVER -DBCMDONGLEHOST -DDHDTHREAD -DBCMWPA2 \
	-DUNRELEASEDCHIP -Dlinux -DDHD_SDALIGN=64 -DMAX_HDR_READ=64  \
	-DDHD_FIRSTREAD=64  -DCUSTOM_OOB_GPIO_NUM=152  -DBCMWAPI_WPI \
	-DUNRELEASEDCHIP -DCUSTOMER_HW -DGET_CUSTOM_MAC_ENABLE -DHUAWEI_QUICK_TOGGLE_WIFI \
	-DDHD_GPL -DDHD_SCHED -DBDC -DTOE -DDHD_BCMEVENTS -DSHOW_EVENTS -DSDIO_ISR_THREAD \
	-DBCMSDIO -DDHD_GPL -DBCMLXSDMMC -DBCMPLATFORM_BUS -DOEM_ANDROID -DEMBEDDED_PLATFORM \
	-DARP_OFFLOAD_SUPPORT -DPKT_FILTER_SUPPORT \
	-Idrivers/net/wireless/bcm4329 -Idrivers/net/wireless/bcm4329/include

DHDOFILES = dhd_linux.o linux_osl.o bcmutils.o dhd_common.o dhd_custom_gpio.o \
	wl_iw.o siutils.o sbutils.o aiutils.o hndpmu.o bcmwifi.o dhd_sdio.o   \
	dhd_linux_sched.o dhd_cdc.o bcmsdh_sdmmc.o bcmsdh.o bcmsdh_linux.o    \
	bcmsdh_sdmmc_linux.o

obj-$(CONFIG_BCM4329) += bcm4329.o
bcm4329-objs += $(DHDOFILES)
EXTRA_CFLAGS = $(DHDCFLAGS)
#EXTRA_LDFLAGS += --strip-debug
