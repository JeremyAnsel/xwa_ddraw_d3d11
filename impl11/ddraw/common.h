// Copyright (c) 2014 J�r�my Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <ddraw.h>
#include <d3d.h>

#include <dxgi.h>
#include <d3d11.h>

#ifdef MINGW_HAS_SECURE_API
// Bug workaround to fix compilation
// of comdef.h
#include <stdio.h>
#endif
#include <comdef.h>

#include <string>
#include <sstream>

#include "ComPtr.h"
#include "logger.h"
#include "utils.h"
#include "config.h"