#include "common.h"
#include "peek_poke.h"
#include "hvcall.h"
#include "mm.h"

#include <psl1ght/lv2.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sysutil/video.h>
#include <rsx/gcm.h>
#include <rsx/reality.h>
#include <io/pad.h>

#include "sconsole.h"


u64 mmap_lpar_addr;
int map_lv1() {
	int result = lv1_undocumented_function_114(0, 0xC, HV_SIZE, &mmap_lpar_addr);
	if (result != 0) {
		PRINTF("Error code %d calling lv1_undocumented_function_114\n", result);
		return 0;
	}
	
	result = mm_map_lpar_memory_region(mmap_lpar_addr, HV_BASE, HV_SIZE, 0xC, 0);
	if (result) {
		PRINTF("Error code %d calling mm_map_lpar_memory_region\n", result);
		return 0;
	}
	
	return 1;
}

void unmap_lv1() {
	if (mmap_lpar_addr != 0)
		lv1_undocumented_function_115(mmap_lpar_addr);
}

void dump_lv1() {
	FILE *f = fopen(DUMP_FILENAME, "wb");
	u64 quad;
	for (u64 i = (u64)HV_BASE; i < HV_BASE + HV_SIZE; i += 8) {
		quad = lv2_peek(i);
		fwrite(quad, 8, 1, f);
	}
	fclose(f);
	PRINTF("Wrote Output to %s\n", DUMP_FILENAME);

}
void dump_lv2() {
	FILE *f = fopen(DUMP2_FILENAME, "wb");
	u64 quad;
	for (u64 i = (u64)LV2_BASE; i < LV2_BASE + LV2_SIZE; i += 8) {
		quad = lv2_peek(i);
		fwrite(quad, 8, 1, f);
	}
	fclose(f);
	PRINTF("Wrote Output to %s\n", DUMP2_FILENAME);

}

void dump_lv1_net() {
	for (u64 i = (u64)HV_BASE; i < HV_BASE + HV_SIZE; i += 8) {
		PRINTF("%16.16lX", lv2_peek(i));
	}
}
void dump_lv2_net() {
	for (u64 i = (u64)LV2_BASE; i < LV2_BASE + LV2_SIZE; i += 8) {
		PRINTF("%16.16lX", lv2_peek(i));
	}

}
void patch_lv2_protection() {
	// changes protected area of lv2 to first byte only
	lv1_poke(0x363a78, 0x0000000000000001ULL);
	lv1_poke(0x363a80, 0xe0d251b556c59f05ULL);
	lv1_poke(0x363a88, 0xc232fcad552c80d7ULL);
	lv1_poke(0x363a90, 0x65140cd200000000ULL);
}
void peek_lv2_protection() {
	PRINTF("%16.16lX \n", lv1_peek(0x363a78));
	PRINTF("%16.16lX \n", lv1_peek(0x363a80));
	PRINTF("%16.16lX \n", lv1_peek(0x363a88));
	PRINTF("%16.16lX \n", lv1_peek(0x363a90));
}

s32 main(s32 argc, const char* argv[]) {
	debug_wait_for_client();
	install_new_poke();
	
	u64 color = FONT_COLOR_WHITE;
	if (!map_lv1()) {
		remove_new_poke();
		//exit(0);
		color = FONT_COLOR_RED;
	} else {
		//patch_lv2_protection();
		dump_lv1_net();
		//dump_lv2_net();
		//peek_lv2_protection();
		unmap_lv1();
		remove_new_poke();
			}			
	PRINTF("done, exiting\n");
	return 0;
}
