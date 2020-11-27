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

#define EXTRA_ENV_SETTINGS CONFIG_EXTRA_ENV_SETTINGS 
#undef CONFIG_EXTRA_ENV_SETTINGS
#define CONFIG_EXTRA_ENV_SETTINGS \
	EXTRA_ENV_SETTINGS \
	"load_bootenv="\
	"load usb ${usbdev}:${usbpart} ${loadaddr} DHupdate.ini;" \
	"echo \"--> Update: found DHupdate.ini (${filesize} bytes)\"; \0"\
	"importbootenv=echo Importing environment from DHupdate.ini...; env import -t ${loadaddr} ${filesize}\0"
#endif

