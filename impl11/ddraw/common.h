// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <ddraw.h>
#include <d3d.h>

#include <dxgi.h>
#include <d3d11.h>

#include <comdef.h>

#include <string>
#include <sstream>

#include "ComPtr.h"
#include "logger.h"
#include "utils.h"
#include "config.h"

// Needed to compile SSE2 instructions
#include <emmintrin.h>
