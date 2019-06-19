#pragma once

/*
 *	FreePIE definitions.
 */

typedef struct freepie_io_6dof_data
{
	float yaw;
	float pitch;
	float roll;

	float x;
	float y;
	float z;
} freepie_io_6dof_data;

// An attempt to access the shared store of data failed.
const INT32 FREEPIE_IO_ERROR_SHARED_DATA = -1;
// An attempt to access out of bounds data was made.
const INT32 FREEPIE_IO_ERROR_OUT_OF_BOUNDS = -2;

// readFreePIE() puts the information in this structure:
extern freepie_io_6dof_data g_FreePIEData;

bool InitFreePIE();
void ShutdownFreePIE();
bool ReadFreePIE();
