#pragma once

#include <SDKDDKVer.h>

#undef NTDDI_VERSION
#undef _WIN32_WINNT
#undef WINVER

#define NTDDI_VERSION		NTDDI_WIN10
#define _WIN32_WINNT		_WIN32_WINNT_WIN10
#define WINVER				_WIN32_WINNT
