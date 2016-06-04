// Copyright (c) 2016 Reimar D�ffinger
// Licensed under the MIT license. See LICENSE.txt

#pragma once

#include "common.h"

extern "C" UINT WINAPI emulJoyGetNumDevs(void);
extern "C" UINT WINAPI emulJoyGetDevCaps(UINT_PTR, struct tagJOYCAPSA *, UINT);
extern "C" UINT WINAPI emulJoyGetPosEx(UINT, struct joyinfoex_tag *);
