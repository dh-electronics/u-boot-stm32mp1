/* SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause */
/*
 * Copyright (C) 2022, Johann Neuhauser <jneuhauser@dh-electronics.com>
 */

#ifndef __MACH_STM32MP_ROM_API_H_
#define __MACH_STM32MP_ROM_API_H_

#ifdef CONFIG_SPL_BUILD
extern uintptr_t rom_api_loc;
#endif

#define ROM_API_OFFSET_AUTH_STATE       0x58
#define ROM_API_OFFSET_ECDSA_VERIFY     0x60

#define ROM_API_AUTH_STATE              ((uint32_t *)(rom_api_loc + ROM_API_OFFSET_AUTH_STATE))
#define ROM_API_AUTH_STATE_NONE         0x0U
#define ROM_API_AUTH_STATE_FAILED       0x1U
#define ROM_API_AUTH_STATE_SUCCESS      0x2U

#define ROM_API_SUCCESS				0x77
#define ROM_API_ECDSA_ALGO_PRIME_256V1		1
#define ROM_API_ECDSA_ALGO_BRAINPOOL_256	2

#endif
