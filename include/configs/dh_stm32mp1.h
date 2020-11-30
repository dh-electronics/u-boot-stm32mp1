/* SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause */
/*
 * Copyright (C) 2020 Marek Vasut <marex@denx.de>
 *
 * Configuration settings for the DH STM32MP15x SoMs
 */

#ifndef __CONFIG_DH_STM32MP1_H__
#define __CONFIG_DH_STM32MP1_H__

#include <configs/stm32mp1.h>

#define CONFIG_SPL_TARGET		"u-boot.itb"
#undef STM32MP_BOOTCMD
#define STM32MP_BOOTCMD "bootcmd_stm32mp=" \
	"if run load_bootenv; then run importbootenv;fi;" \
	"echo \"Boot over ${boot_device}${boot_instance}!\";" \
	"if test ${boot_device} = serial || test ${boot_device} = usb;" \
	"then stm32prog ${boot_device} ${boot_instance}; " \
	"else " \
		"run env_check;" \
		"if test ${boot_device} = mmc;" \
		"then env set boot_targets \"mmc${boot_instance}\"; fi;" \
		"if test ${boot_device} = nand ||" \
		  " test ${boot_device} = spi-nand ;" \
		"then env set boot_targets ubifs0; fi;" \
		"run distro_bootcmd;" \
	"fi;\0" 

#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootdelay=1\0" \
	"kernel_addr_r=0xc2000000\0" \
	"fdt_addr_r=0xc4000000\0" \
	"scriptaddr=0xc4100000\0" \
	"pxefile_addr_r=0xc4200000\0" \
	"splashimage=0xc4300000\0"  \
	"ramdisk_addr_r=0xc4400000\0" \
	"altbootcmd=run bootcmd\0" \
	"env_default=1\0" \
	"env_check=if test $env_default -eq 1;"\
		" then env set env_default 0;env save;fi\0" \
	STM32MP_BOOTCMD \
	BOOTENV \
	"boot_net_usb_start=true\0" \
	"usb_pgood_delay=1000\0" \
	"update_sf=" /* Erase SPI NOR and install U-Boot from SD */	\
		"setexpr loadaddr1 ${loadaddr} + 0x1000000 && "		\
		"load mmc 0:4 ${loadaddr1} /boot/u-boot-spl.stm32 && "	\
		"env set filesize1 ${filesize} && "			\
		"load mmc 0:4 ${loadaddr} /boot/u-boot.itb && "		\
		"sf probe && sf erase 0 0x200000 && "			\
		"sf update ${loadaddr1} 0 ${filesize1} && "		\
		"sf update ${loadaddr1} 0x40000 ${filesize1} && "	\
		"sf update ${loadaddr} 0x80000 ${filesize} && "		\
		"env set filesize1 && env set loadaddr1\0" \
	"load_bootenv="\
	"usb stop; usb start; if load usb ${usbdev}:${usbpart} ${loadaddr} DHupdate.ini;" \
	"then echo \"--> Update: found DHupdate.ini (${filesize} bytes)\"; fi;\0"\
	"importbootenv=echo Importing environment from DHupdate.ini...; env import -t ${loadaddr} ${filesize}\0"\
	"usbdev=0\0"\
	"usbpart=1\0"
#endif

