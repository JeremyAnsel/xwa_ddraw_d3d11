#include <windows.h>
#include "FreePIE.h"

void log_debug(const char *format, ...);

typedef UINT32(__cdecl *freepie_io_6dof_slots_fun_type)();
typedef INT32(__cdecl *freepie_io_6dof_read_fun_type)(UINT32 index, UINT32 length, freepie_io_6dof_data *output);

freepie_io_6dof_slots_fun_type freepie_io_6dof_slots = NULL;
freepie_io_6dof_read_fun_type freepie_io_6dof_read = NULL;

bool bFreePIEInitialized = false;
freepie_io_6dof_data g_FreePIEData;
HMODULE hFreePIE = NULL;

bool InitFreePIE() {
	LONG lRes = ERROR_SUCCESS;
	char regvalue[1024];
	DWORD size = 1024;
	log_debug("[DBG] Initializing FreePIE");
	bFreePIEInitialized = false;

	lRes = RegGetValueA(HKEY_CURRENT_USER, "Software\\FreePIE", "path", RRF_RT_ANY, NULL, regvalue, &size);
	if (lRes != ERROR_SUCCESS) {
		log_debug("[DBG] Registry key for FreePIE was not found, error: 0x%x", lRes);
		return false;
	}

	if (size > 0) {
		log_debug("[DBG] FreePIE path: %s", regvalue);
		SetDllDirectory(regvalue);
	}
	else {
		log_debug("[DBG] Cannot load FreePIE, registry path is empty!");
		return false;
	}

	hFreePIE = LoadLibraryA("freepie_io.dll");

	if (hFreePIE != NULL) {
		log_debug("[DBG] FreePIE loaded");
		freepie_io_6dof_slots = (freepie_io_6dof_slots_fun_type)GetProcAddress(hFreePIE, "freepie_io_6dof_slots");
		freepie_io_6dof_read = (freepie_io_6dof_read_fun_type)GetProcAddress(hFreePIE, "freepie_io_6dof_read");

		if (freepie_io_6dof_slots == NULL || freepie_io_6dof_read == NULL) {
			log_debug("[DBG] Could not load all of FreePIE's functions");
			return false;
		}

		bFreePIEInitialized = true;
		return true;
	}
	else {
		log_debug("[DBG] Could not load FreePIE");
		return false;
	}
	bFreePIEInitialized = true;
	return true;
}

void ShutdownFreePIE() {
	log_debug("[DBG] Shutting down FreePIE");
	if (hFreePIE != NULL)
		FreeLibrary(hFreePIE);
}

bool ReadFreePIE(int slot) {
	if (!bFreePIEInitialized) {
		//log_debug("[DBG] FreePIE is NOT initialized");
		return false;
	}
	// Check how many slots (values) the current FreePIE implementation provides.
	int error = freepie_io_6dof_read(slot, 1, &g_FreePIEData);
	if (error < 0) {
		log_debug("[DBG] FreePIE error: %d", error);
		return false;
	}
	return true;
}
