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

#define DH_EXTRA_ENV_SETTINGS \
	"load_bootenv="\
	"load usb ${usbdev}:${usbpart} ${loadaddr} DHupdate.ini;" \
	"echo \"--> Update: found DHupdate.ini (${filesize} bytes)\"; \0"\
	"importbootenv=echo Importing environment from DHupdate.ini...; env import -t ${loadaddr} ${filesize}\0"
#endif

