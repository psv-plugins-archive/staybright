/*
This file is part of Staybright
Copyright © 2020 浅倉麗子

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// 六花ＰＲＯＪＥＣＴ

#include <string.h>

#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/power.h>

#include <taihen.h>

#define USED __attribute__ ((used))
#define UNUSED __attribute__ ((unused))

#define GLZ(x) do {\
	if ((x) < 0) { goto fail; }\
} while (0)

#define N_HOOK 1
static SceUID hook_id[N_HOOK];
static tai_hook_ref_t hook_ref[N_HOOK];

static SceUID hook_export(int idx, char *mod, int libnid, int funcnid, void *func) {
	SceUID ret = taiHookFunctionExportForKernel(KERNEL_PID, hook_ref+idx, mod, libnid, funcnid, func);
	if (ret >= 0) {
		hook_id[idx] = ret;
	}
	return ret;
}
#define HOOK_EXPORT(idx, mod, libnid, funcnid, func)\
	hook_export(idx, mod, libnid, funcnid, func##_hook)

static int UNHOOK(int idx) {
	int ret = 0;
	if (hook_id[idx] >= 0) {
		ret = taiHookReleaseForKernel(hook_id[idx], hook_ref[idx]);
		if (ret == 0) {
			hook_id[idx] = -1;
			hook_ref[idx] = -1;
		}
	}
	return ret;
}

static int kscePowerSetIdleTimer_hook(int callback_slot, SceUInt64 time) {
	time = callback_slot == 1 ? 0 : time;
	return TAI_CONTINUE(int, hook_ref[0], callback_slot, time);
}

static void startup(void) {
	memset(hook_id, 0xFF, sizeof(hook_id));
	memset(hook_ref, 0xFF, sizeof(hook_ref));
}

static void cleanup(void) {
	for (int i = 0; i < N_HOOK; i++) { UNHOOK(i); }
}

USED int module_start(UNUSED SceSize args, UNUSED const void *argp) {
	startup();
	GLZ(HOOK_EXPORT(0, "ScePower", 0x1590166F, 0x6BC26FC7, kscePowerSetIdleTimer));
	kscePowerSetIdleTimer(1, 0);
	return SCE_KERNEL_START_SUCCESS;

fail:
	cleanup();
	return SCE_KERNEL_START_FAILED;
}

USED int module_stop(UNUSED SceSize args, UNUSED const void *argp) {
	cleanup();
	return SCE_KERNEL_STOP_SUCCESS;
}
