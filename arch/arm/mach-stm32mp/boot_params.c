// SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause
/*
 * Copyright (C) 2019, STMicroelectronics - All Rights Reserved
 */

#define LOG_CATEGORY LOGC_ARCH

#include <common.h>
#include <log.h>
#include <linux/libfdt.h>
#include <asm/sections.h>
#include <asm/system.h>
#include <asm/arch/rom_api.h>

/*
 * Without forcing the ".data" section, this would get saved in ".bss". BSS
 * will be cleared soon after, so it's not suitable.
 */
#ifdef CONFIG_SPL_BUILD
uintptr_t rom_api_loc __section(".data");
#endif

/*
 * Force data-section, as .bss will not be valid
 * when save_boot_params is invoked.
 */
#ifdef CONFIG_TFABOOT
static unsigned long nt_fw_dtb __section(".data");
#endif

/*
 * This function is called from start.S
 */
void save_boot_params(unsigned long r0, unsigned long r1, unsigned long r2,
		      unsigned long r3)
{
	/*
	* The ROM gives us the API location in r0 when starting. This is only available
	* during SPL, as there isn't (yet) a mechanism to pass this on to u-boot.
	*/
#ifdef CONFIG_SPL_BUILD
	rom_api_loc = r0;
#endif

	/*
	 * Save the FDT address provided by TF-A in r2 at boot time.
	 */
#ifdef CONFIG_TFABOOT
	nt_fw_dtb = r2;
#endif

	save_boot_params_ret();
}

/*
 * Use the saved FDT address provided by TF-A at boot time (NT_FW_CONFIG =
 * Non Trusted Firmware configuration file) when the pointer is valid
 */
#ifdef CONFIG_TFABOOT
void *board_fdt_blob_setup(int *err)
{
	log_debug("%s: nt_fw_dtb=%lx\n", __func__, nt_fw_dtb);

	*err = 0;
	/* use external device tree only if address is valid */
	if (nt_fw_dtb >= STM32_DDR_BASE) {
		if (fdt_magic(nt_fw_dtb) == FDT_MAGIC)
			return (void *)nt_fw_dtb;
		log_debug("%s: DTB not found.\n", __func__);
	}
	log_debug("%s: fall back to builtin DTB, %p\n", __func__, &_end);

	return (void *)&_end;
}
#endif